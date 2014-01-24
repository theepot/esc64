#include "service/ComputerControlService.h"
#include "SimControl.hpp"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <sstream>
#include <VpiUtils.hpp>
#include <stdint.h>
#include <VirtualIOExtension.hpp>
#include <RAM.hpp>
#include <cstdlib>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::esc64controlsrv;
using namespace ::VpiUtils;

using boost::shared_ptr;

namespace po = boost::program_options;

static TNonblockingServer* server_ptr; //TODO: dehackify

class ServiceImpl : virtual public ComputerControlServiceIf
{
public:
	ServiceImpl();
	virtual ~ServiceImpl() { }

	//thrift service implementation
	ComputerState::type getState();
	void start();
	void pause();
	void step();
	void microStep();
	void reset();
	void quit();
	CarryState::type getCarryFlag();
	bool getZeroFlag();
	void getRegister(std::vector<int32_t> & _return, const int32_t offset, const int32_t size);
	void getMemory(std::vector<int32_t> & _return, const int32_t offset, const int32_t size);
	int64_t getInstrCount();
	int64_t getClockCount();

	//verilog tasks
	void tickTask();
	void startSimControlTask();

private:
	static const size_t REG_HANDLES_SIZE = 8;
	static const int PORT = 9090;

	enum SimControlState {
		RUNNING = 0,
		PAUSED,
		STARTING_STEP,
		STEPPING,
		MICRO_STEPPING,
		QUITING
	};

	boost::thread* serviceThread;
	SimControlState prevSimState;
	SimControlState simState;
	boost::mutex simStateMutex;
	boost::condition_variable simStateCond;
	boost::mutex simMutex;
	int64_t instr_counter;
	int64_t clock_counter;
	bool ignore_at_fetch;

	vpiHandle carryFlagHandle;
	vpiHandle zeroFlagHandle;
	vpiHandle regHandles[REG_HANDLES_SIZE];
	vpiHandle ramHandle;
	vpiHandle atFetchHandle;
	vpiHandle stateRegHandle;

	void parseOptions(int argc, char** argv);
	void setState(SimControlState state);
	void initModuleHandles();
	void serverThreadProc();
};

struct VerilogTask
{
	const char* name;
	void (ServiceImpl::*func)();
	PLI_INT32 taskOrFunc;
};

static const VerilogTask VERILOG_TASKS[] =
{
	{ "$tick", &ServiceImpl::tickTask, vpiSysTask},
	{ "$start_sim_control", &ServiceImpl::startSimControlTask, vpiSysTask},
};

static const size_t VERILOG_TASKS_SIZE = sizeof VERILOG_TASKS / sizeof (VerilogTask);

///// implementation /////
ServiceImpl::ServiceImpl() {
	ignore_at_fetch = true;
	instr_counter = 0;
	clock_counter = 0;
}

void ServiceImpl::start()
{
	setState(RUNNING);
}

void ServiceImpl::pause()
{
	setState(PAUSED);
}

void ServiceImpl::step()
{
	setState(STARTING_STEP);

	boost::unique_lock<boost::mutex> lock(simStateMutex);
	while(simState != PAUSED)
	{
		simStateCond.wait(lock);
	}
}

void ServiceImpl::microStep()
{
	setState(MICRO_STEPPING);

	boost::unique_lock<boost::mutex> lock(simStateMutex);
	while(simState != PAUSED)
	{
		simStateCond.wait(lock);
	}
}

void ServiceImpl::reset()
{
	//boost::lock_guard<boost::mutex> lock(simMutex);
	//instr_counter = 0;
	//clock_counter = 0;
	//ignore_at_fetch = true;
	//TODO
	std::cout << "reset() is not supported! Restart the simulation\n";
}

void ServiceImpl::getRegister(std::vector<int32_t> & _return, const int32_t offset, const int32_t size)
{
	unsigned off = offset;
	unsigned sz = size;

	if(off >= REG_HANDLES_SIZE || off + sz > REG_HANDLES_SIZE)
	{
		fprintf(stderr, "SimControl: WARNING: client requested out of range registers\n");
		return;
	}

	boost::lock_guard<boost::mutex> lock(simMutex);

	s_vpi_value val;
	val.format = vpiIntVal;

	for(vpiHandle* h = regHandles + offset; h < regHandles + offset + sz; ++h)
	{
		vpi_get_value(*h, &val);
		_return.push_back(val.value.integer);
	}
}

void ServiceImpl::getMemory(std::vector<int32_t> & _return, const int32_t offset, const int32_t size)
{
	if(offset < 0 || size < 0 || offset + size > (1 << 16))
	{
		fprintf(stderr, "SimControl: WARNING: client requested out of range memory\n");
		return;
	}

	boost::lock_guard<boost::mutex> lock(simMutex);
	for(int i = offset; i < offset+size; ++i)
	{
		BitVector16 bv = VirtualIOExtension::mainRAM->getByte(i);
		int32_t x = ((bv.a & 0xFF) << 8) | (bv.b & 0xFF);
		_return.push_back(x);
	}
}

void ServiceImpl::tickTask()
{
	ArgumentIterator args;
	PLI_INT32 at_fetch = args.NextInt();
	SimControlState s;

	if(at_fetch) {
		instr_counter++;
	}

	if(ignore_at_fetch && at_fetch) {
		if(instr_counter == 2) {
			ignore_at_fetch = false;
			instr_counter = 1;
		}
	}

	bool loop = true;
	while(loop)
	{
		//pause on status not OK.
		//TODO: breakpoints

		{
			boost::lock_guard<boost::mutex> lock(simStateMutex);
			if(simState == QUITING) {
				loop = false;
				server_ptr->stop();
				serviceThread->join();
				vpi_control(vpiFinish);
				break;
			}
		}

		s_vpi_value val;
		val.format = vpiIntVal;
		vpi_get_value(stateRegHandle, &val);
		switch(val.value.integer)
		{
		case 0: break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			setState(PAUSED);
			break;
		default: assert(0); break;
		}

		{
			boost::lock_guard<boost::mutex> lock(simStateMutex);
			if(prevSimState != simState)
			{
				prevSimState = simState;
			}
			s = simState;
		}
		simStateCond.notify_all();

		switch(s)
		{
		case RUNNING:

			loop = false;
			break;
		case PAUSED:
			{
				simMutex.unlock();
				boost::unique_lock<boost::mutex> lock(simStateMutex);
				if(simState == s)
				{
					simStateCond.wait(lock);
				}
				simMutex.lock();
			}
			break;

		case STARTING_STEP:
			setState(STEPPING);
			loop = false;
			break;

		case STEPPING:
			if(!ignore_at_fetch && at_fetch)
			{
				setState(PAUSED);
				break;
			}
			loop = false;
			break;
		case MICRO_STEPPING:
			setState(PAUSED);
			loop = false;
			break;
		case QUITING:
			break;
		default:
			throw std::runtime_error("Illegal simulation state");
		}
	}

	clock_counter++;
}

void ServiceImpl::startSimControlTask()
{
	assert(serviceThread == NULL);

	//get module handles
	initModuleHandles();
	
	//get cmd arguments
	s_vpi_vlog_info vlog_info;
	vpi_get_vlog_info(&vlog_info);
	parseOptions(vlog_info.argc, vlog_info.argv);

	simMutex.lock();

	//start server thread
	serviceThread = new boost::thread(boost::bind(&ServiceImpl::serverThreadProc, this));
}

void ServiceImpl::parseOptions(int argc, char** argv)
{
	po::options_description desc( "Allowed options");

	desc.add_options()
	    ("paused", "start the simulation paused");

	boost::program_options::basic_command_line_parser<char> bcp = boost::program_options::basic_command_line_parser<char>(argc, argv);
	bcp.options(desc);
	bcp.allow_unregistered();

	po::variables_map vm;
	po::store(bcp.run(), vm);
	po::notify(vm);

	prevSimState = simState = (vm.count("paused") ? PAUSED : RUNNING);
}

void ServiceImpl::setState(SimControlState state)
{
	{
		boost::lock_guard<boost::mutex> lock(simStateMutex);
		prevSimState = simState;
		simState = state;
	}

	simStateCond.notify_all();
	//TODO: print state changes when usefull to know
}

void ServiceImpl::initModuleHandles()
{
	VpiIterator it(vpiModule, NULL);
	vpiHandle top = it.Next();

	//registers
	for(size_t i = 0; i < REG_HANDLES_SIZE; ++i)
	{
		std::stringstream ss;
		ss << "reg" << i;
		regHandles[i] = vpi_handle_by_name(ss.str().c_str(), top);
		assert(regHandles[i]);
	}

	//flags
	carryFlagHandle = vpi_handle_by_name("carry", top);
	assert(carryFlagHandle);
	zeroFlagHandle = vpi_handle_by_name("zero", top);
	assert(zeroFlagHandle);

	//status
	stateRegHandle = vpi_handle_by_name("state", top);
	assert(stateRegHandle);

	//at fetch
	atFetchHandle = vpi_handle_by_name("at_fetch", top);
	assert(atFetchHandle);
}


void ServiceImpl::serverThreadProc()
{
	shared_ptr<ServiceImpl> handler(this);
	shared_ptr<TProcessor> processor(new ComputerControlServiceProcessor(handler));
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	TNonblockingServer server(processor, protocolFactory, PORT);
	server_ptr = &server;

	server.serve();
}

ComputerState::type ServiceImpl::getState() {
	boost::lock_guard<boost::mutex> lock(simStateMutex);
	boost::lock_guard<boost::mutex> lock2(simMutex);
	switch(simState)
	{
	case RUNNING: return ComputerState::RUNNING;
	case PAUSED:
	case STARTING_STEP:
	case STEPPING:
	case MICRO_STEPPING:
		s_vpi_value val;
		val.format = vpiIntVal;
		vpi_get_value(stateRegHandle, &val);
		switch(val.value.integer)
		{
		case 0: return ComputerState::PAUSED;
		case 1: return ComputerState::UNKNOWN_OPCODE;
		case 2: return ComputerState::HW_FAULT;
		case 3: return ComputerState::HW_FAULT;
		case 4: return ComputerState::HALT_INSTR;
		case 5: return ComputerState::IO_ERROR;
		default: assert(0); break;
		}
		break;
	default: assert(0); break;
	}

	assert(0);
}

CarryState::type ServiceImpl::getCarryFlag() {
	boost::lock_guard<boost::mutex> lock(simMutex);
	s_vpi_value val;
	val.format = vpiIntVal;
	vpi_get_value(carryFlagHandle, &val);
	if(val.value.integer) {
		return CarryState::SET;
	} else {
		return CarryState::UNSET;
	}
}

bool ServiceImpl::getZeroFlag() {
	boost::lock_guard<boost::mutex> lock(simMutex);
	s_vpi_value val;
	val.format = vpiIntVal;
	vpi_get_value(zeroFlagHandle, &val);
	return val.value.integer != 0;
}

int64_t ServiceImpl::getInstrCount() {
	boost::lock_guard<boost::mutex> lock(simMutex);
	return instr_counter;
}

int64_t ServiceImpl::getClockCount() {
	boost::lock_guard<boost::mutex> lock(simMutex);
	return clock_counter;
}

void ServiceImpl::quit() {
	setState(QUITING);
}

void SimControl_entry(void)
{
	ServiceImpl* s = new ServiceImpl();

	for(const VerilogTask* t = VERILOG_TASKS; t < VERILOG_TASKS + VERILOG_TASKS_SIZE; ++t)
	{
		registerSysTF(t->name, boost::bind(t->func, s), t->taskOrFunc);
	}
}

