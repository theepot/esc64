#include "ComputerControlService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/thread.hpp>
#include <RAM.hpp>
#include <VirtualIOStream.hpp>
#include <VirtualIO.hpp>
#include <ESC64.hpp>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
}

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::esc64controlsrv;
using namespace ::virtual_io;

enum RunningState {
	RUNNING,
	PAUSED,
	QUITING,
};

class ControlServiceHandler : virtual public ComputerControlServiceIf {
private:
	ESC64* esc64;
	RAM* ram;
	boost::mutex* esc64_mutex; //also protects ram, devices and do_quit
	RunningState* runningState;
	boost::mutex* runningState_mutex;
	boost::condition_variable* cv;

public:
	bool do_quit;
	ControlServiceHandler(ESC64* esc64, RAM* ram, boost::mutex* esc64_mutex, RunningState* runningState, boost::mutex* runningState_mutex, boost::condition_variable* cv) :
		esc64(esc64),
		ram(ram),
		esc64_mutex(esc64_mutex),
		runningState(runningState),
		runningState_mutex(runningState_mutex),
		cv(cv) {
	}

	RunningState getRunningState() {
		boost::lock_guard<boost::mutex> lock(*runningState_mutex);
		return *runningState;
	}

	ComputerState::type getState() {
		RunningState r = getRunningState();

		if(r == RUNNING) {
			return ComputerState::RUNNING;
		} else if (r == PAUSED) {
			boost::lock_guard<boost::mutex> lock2(*esc64_mutex);
			switch(esc64->state) {
				case ESC64::OK:
					return ComputerState::PAUSED;
				case ESC64::UNKNOWN_OPCODE:
					return ComputerState::UNKNOWN_OPCODE;
				case ESC64::HALT_INSTR:
					return ComputerState::HALT_INSTR;
				case ESC64::IO_ERROR:
					return ComputerState::IO_ERROR;
				case ESC64::UNDEFINED_EFFECT:
					return ComputerState::UNDEFINED_EFFECT;
				default:
					assert(0);
					break;
			}
		}

		assert(0);

		return ComputerState::RUNNING;
	}

	void start() {
		runningState_mutex->lock();
		*runningState = RUNNING;
		runningState_mutex->unlock();

		cv->notify_all();
	}

	void pause() {
		runningState_mutex->lock();
		*runningState = PAUSED;
		runningState_mutex->unlock();

		cv->notify_all();
	}

	void microStep() {
		//not implementable :D
	}

	void step() {
		if(getRunningState() != RUNNING) {
			boost::lock_guard<boost::mutex> lock2(*esc64_mutex);

			esc64->step();
		}
		cv->notify_all();
	}

	void reset() {
		boost::lock_guard<boost::mutex> lock(*esc64_mutex);
		esc64->reset();
		cv->notify_all();
	}

	CarryState::type getCarryFlag() {
		boost::lock_guard<boost::mutex> lock(*esc64_mutex);
		if(esc64->c_flag_is_defined) {
			return esc64->c_flag ? CarryState::SET : CarryState::UNSET;
		} else {
			return CarryState::UNDEFINED;
		}
	}

	bool getZeroFlag() {
		boost::lock_guard<boost::mutex> lock(*esc64_mutex);
		return esc64->z_flag;
	}

	void getRegister(std::vector<int32_t> & _return, const int32_t offset, const int32_t size) {
		if(offset < 0 || size < 0 || offset + size > 8) {
			fprintf(stderr, "esc64vm: WARNING: client requested out of range memory\n");
			return;
		}
		boost::lock_guard<boost::mutex> lock(*esc64_mutex);

		for(int i = offset; i < offset+size; ++i) {
			_return.push_back(esc64->regs[i]);
		}
	}

	void getMemory(std::vector<int32_t> & _return, const int32_t offset, const int32_t size) {
		if(offset < 0 || size < 0 || offset + size > (1 << 16))
		{
			fprintf(stderr, "esc64vm: WARNING: client requested out of range memory\n");
			return;
		}

		boost::lock_guard<boost::mutex> lock(*esc64_mutex);
		for(int i = offset; i < offset+size; ++i)
		{
			BitVector16 bv = ram->getByte(i);
			int32_t x = ((bv.a & 0xFF) << 8) | (bv.b & 0xFF);
			_return.push_back(x);
		}

	}

	int64_t getInstrCount() {
		if(getRunningState() == PAUSED) {
			boost::lock_guard<boost::mutex> lock(*esc64_mutex);
			return esc64->get_step_count();
		}
		return -1;
	}

	int64_t getClockCount() {
		return 0;
	}

	void quit(void) {
		boost::lock_guard<boost::mutex> lock(*runningState_mutex);
		*runningState = QUITING;

		cv->notify_all();
	}

};



int main(int argc, char **argv) {
	bool quit_after_halt = false;
	VirtualIOManager viom;
	viom.print_io_activity = false;
	bool start_paused = false;
	RAM ram(false, 0, (1 << 15) - 1);

	std::string vis = "-";
	std::string vos = "-";

	int port = 9090;
	bool found_ram_argument = false;
	for(int i = 1; i < argc; ++i) {
		if(std::string(argv[i]) == "-r") {
			found_ram_argument = true;
			if(i + 1 >= argc) {
				fprintf(stderr, "-r needs an argument\n");
				return 1;
			} else {
				FILE* f = fopen(argv[i + 1], "r");
				if(f != NULL) {
					ram.load_from_verilog_file(f);
					fclose(f);
				} else {
					fprintf(stderr, "could not open file %s for reading\n", argv[i + 1]);
				}
			}
		} else if(std::string(argv[i]) == "--paused") {
			start_paused = true;
		} else if(std::string(argv[i]) == "--port") {
			if(i + 1 >= argc) {
				fprintf(stderr, "--port needs an argument\n");
				return 1;
			} else {
				port = strtol(argv[i+1], NULL, 10);
			}
		} else if(std::string(argv[i]) == "--quit-after-halt") {
			quit_after_halt = true;
		} else if(std::string(argv[i]) == "--vis") {
			if(i + 1 >= argc) {
				fprintf(stderr, "--vis needs an argument\n");
				return 1;
			} else {
				vis = argv[i+1];
			}
		} else if(std::string(argv[i]) == "--vos") {
			if(i + 1 >= argc) {
				fprintf(stderr, "--vos needs an argument\n");
				return 1;
			} else {
				vos = argv[i+1];
			}
		}
	}

	int visfileno, vosfileno;
	if(vis == "-") {
		visfileno = fileno(stdin);
	} else {
		visfileno = open(vis.c_str(), O_RDONLY);
		if(visfileno == -1) {
			fprintf(stderr, "ERROR: could not open %s for reading: %s\n", vis.c_str(), strerror(errno));
			return 1;
		}
	}

	if(vos == "-") {
		vosfileno = fileno(stdout);
	} else {
		vosfileno = open(vos.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0640);
		if(vosfileno == -1) {
			fprintf(stderr, "ERROR: could not open %s for writing: %s\n", vos.c_str(), strerror(errno));
			return 1;
		}
	}

	VirtualIOStream vios(0xAAAA >> 1, visfileno, vosfileno);

	if(port <= 0 || port > 65535) {
		fprintf(stderr, "ERROR: invalid thrift server port: %d\n", port);
		return 1;
	}

	if(!found_ram_argument) {
		fprintf(stderr, "WARNING: no ram image defined\n");
	}

	viom.add_device(&ram);
	viom.add_device(&vios);
	ESC64 esc64(&viom);
	esc64.reset();

	boost::mutex esc64_mutex; //also protects ram and devices
	RunningState runningState = start_paused ? PAUSED : RUNNING;
	boost::mutex runningState_mutex;
	boost::condition_variable cv;

	shared_ptr<ControlServiceHandler> handler(new ControlServiceHandler(&esc64, &ram, &esc64_mutex, &runningState, &runningState_mutex, &cv));
	shared_ptr<TProcessor> processor(new ComputerControlServiceProcessor(handler));
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	TNonblockingServer server(processor, protocolFactory, port);

	boost::thread serverThread(boost::bind(&TNonblockingServer::serve, server));

	boost::unique_lock<boost::mutex> lock(esc64_mutex);

	//disable line-buffering on stdin
	struct termios old_tio, new_tio;
	assert(!tcgetattr(STDIN_FILENO, &old_tio));
	new_tio = old_tio;
	new_tio.c_lflag &= (~ICANON & ~ECHO);
	assert(!tcsetattr(STDIN_FILENO, TCSANOW, &new_tio));

	for(;;) {

		if(runningState == QUITING || (quit_after_halt && esc64.state == ESC64::HALT_INSTR)) {
			server.stop();
			cv.notify_all();
			break;
		}

		RunningState rs;
		runningState_mutex.lock();
		rs = runningState;
		runningState_mutex.unlock();

		assert(!(rs == RUNNING && !esc64.state == ESC64::OK));

		if(rs != RUNNING) {
			cv.wait(lock);
			continue;
		}

		assert(esc64.state == ESC64::OK);
		esc64.step();
		if(esc64.state != ESC64::OK) {
			runningState_mutex.lock();
			runningState = PAUSED;
			runningState_mutex.unlock();
		}
		cv.notify_all();
	}

	//restore old settings
	tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);

	close(visfileno);
	close(vosfileno);

	return 0;
}

