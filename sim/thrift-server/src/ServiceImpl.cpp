#include "service/SimService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/array.hpp>
#include <sstream>
#include <VpiUtils.hpp>

#define BIND_TASK(a, b)	static int a(PLI_BYTE8* p) { reinterpret_cast<ServiceImpl*>(p)->b(); return 0; }

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::esc64simsrv;
using namespace ::VpiUtils;

using boost::shared_ptr;

namespace po = boost::program_options;

class ServiceImpl : virtual public SimServiceIf
{
public:
	ServiceImpl() { };
	virtual ~ServiceImpl() { }

	//thrift service implementation
	SimState::type getState();
	void start();
	void pause();
	void step();
	void microStep();
	ErrCode::type getErrCode();
	void reset();
	void getRegister(std::vector<int32_t> & _return, const int32_t offset, const int32_t size);
	void getMemory(std::vector<int32_t> & _return, const int32_t offset, const int32_t size);

	//verilog tasks
	void tickTask();
	void printNumTask(); //still here for reference purposes
	void startServerTask();
	void haltTask();
	void errorTask();

private:
	static const size_t REG_HANDLES_SIZE = 8;
	static const int PORT = 9090;

	boost::thread* serviceThread;
	SimState::type prevSimState;
	SimState::type simState;
	boost::mutex simStateMutex;
	boost::condition_variable simStateCond;
	boost::mutex simMutex;

	ErrCode::type simError;

	vpiHandle regHandles[REG_HANDLES_SIZE];
	vpiHandle ramHandle;

	vpiHandle microStepHandle;

	void parseOptions(int argc, char** argv);
	void setState(SimState::type state);
	void initModuleHandles();
	void serverThreadProc();
	void setMicroStep();
};


///// verilog task bindings & startup routines /////
static void registerTasks();

struct VerilogTask
{
	const char* name;
	PLI_INT32 (*calltf)(PLI_BYTE8*);
};

BIND_TASK(tickTask_, tickTask)
BIND_TASK(printNumTask_, printNumTask)
BIND_TASK(startServerTask_, startServerTask)
BIND_TASK(haltTask_, haltTask)
BIND_TASK(errorTask_, errorTask)

static const VerilogTask VERILOG_TASKS[] =
{
	{ "$tick", tickTask_ },
	{ "$print_add", printNumTask_ },
	{ "$start_thrift_server", startServerTask_ },
	{ "$halt", haltTask_ },
	{ "$error", errorTask_ }
};

static const size_t VERILOG_TASKS_SIZE = sizeof VERILOG_TASKS / sizeof (VerilogTask);

void (*vlog_startup_routines[])() = { registerTasks, 0 };


///// implementation /////
SimState::type ServiceImpl::getState()
{
	boost::lock_guard<boost::mutex> lock(simStateMutex);
	return simState;
}

void ServiceImpl::start()
{
	setState(SimState::RUNNING);
}

void ServiceImpl::pause()
{
	setState(SimState::PAUSED);
}

void ServiceImpl::step()
{
	setState(SimState::STEPPING);

	boost::unique_lock<boost::mutex> lock(simStateMutex);
	while(simState == SimState::STEPPING)
	{
		simStateCond.wait(lock);
	}
}

void ServiceImpl::microStep()
{
	setState(SimState::MICRO_STEPPING);

	boost::unique_lock<boost::mutex> lock(simStateMutex);
	while(simState == SimState::STEPPING)
	{
		simStateCond.wait(lock);
	}
}

ErrCode::type ServiceImpl::getErrCode()
{
	boost::lock_guard<boost::mutex> lock(simMutex);
	return simError;
}

void ServiceImpl::reset()
{
	boost::lock_guard<boost::mutex> lock(simMutex);
	//TODO
	std::cout << "reset() is not supported!\n";
}

void ServiceImpl::getRegister(std::vector<int32_t> & _return, const int32_t offset, const int32_t size)
{
	unsigned off = offset;
	unsigned sz = size;

	if(off >= REG_HANDLES_SIZE || off + sz > REG_HANDLES_SIZE)
	{
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
	unsigned off = offset;
	unsigned sz = size;

	if(off > 0xFFFF || off + sz > 0x10000)
	{
		return;
	}

	boost::lock_guard<boost::mutex> lock(simMutex);

	s_vpi_value val;
	val.format = vpiIntVal;

	for(unsigned i = off; i < off + sz; ++i)
	{
		vpiHandle h = vpi_handle_by_index(ramHandle, i);
		assert(h);
		vpi_get_value(h, &val);
		_return.push_back(val.value.integer);
	}
}

void ServiceImpl::tickTask()
{
	SimState::type s;

	for(;;)
	{
		{
			boost::lock_guard<boost::mutex> lock(simStateMutex);
			if(prevSimState != simState)
			{
				printf("Simulation state changed from %s to %s\n", _SimState_VALUES_TO_NAMES.at(prevSimState), _SimState_VALUES_TO_NAMES.at(simState));
				prevSimState = simState;
			}
			s = simState;
		}
		simStateCond.notify_all();

		switch(s)
		{
		case SimState::RUNNING:
			//TODO pause on error, halt or breakpoint
			return;

		case SimState::HALTED:
		case SimState::PAUSED:
			{
				simMutex.unlock();
				boost::unique_lock<boost::mutex> lock(simStateMutex);
				while(simState == s)
				{
					simStateCond.wait(lock);
				}
				simMutex.lock();
			}
			break;

		case SimState::STEPPING:
			setState(SimState::PAUSED);
			return;
			
		case SimState::MICRO_STEPPING:
			setMicroStep();
			setState(SimState::PAUSED);
			return;

		default:
			throw std::runtime_error("Illegal simulation state");
		}
	}
}

void ServiceImpl::printNumTask()
{
	ArgumentIterator it;

	PLI_INT32 a = it.NextInt();
	PLI_INT32 b = it.NextInt();

	vpi_printf("a(%d) + b(%d) = %d\n", a, b, a + b);
}

void ServiceImpl::startServerTask()
{
	assert(serviceThread == NULL);

	//get module handles
	initModuleHandles();
	
	//get cmd arguments
	bool start_paused = false;
	s_vpi_vlog_info vlog_info;
	vpi_get_vlog_info(&vlog_info);
	parseOptions(vlog_info.argc, vlog_info.argv);

	//initialize simulation state / sync
	if(start_paused)
	{
		prevSimState = simState = SimState::HALTED;
	}
	else 
	{
		prevSimState = simState = SimState::RUNNING;
	}
	simMutex.lock();

	//start server thread
	serviceThread = new boost::thread(boost::bind(&ServiceImpl::serverThreadProc, this));
}

void ServiceImpl::haltTask()
{
	setState(SimState::HALTED);
}

void ServiceImpl::errorTask()
{
	setState(SimState::HALTED);

//	vpiHandle systf = vpi_handle(vpiSysTfCall, NULL);
//	VpiIterator it(vpiArgument, systf);
//	ArgumentIterator argIt(&it);

	ArgumentIterator it;

	PLI_INT32 e = it.NextInt();
	simError = static_cast<ErrCode::type>(e);
}

void ServiceImpl::parseOptions(int argc, char** argv)
{
	po::options_description desc("Allowed options");
	desc.add_options()
	    ("paused", "start the simulation paused")
	    ("lst", po::value<std::string>(), "program lst file");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	simState = vm.count("paused") ? SimState::HALTED : SimState::RUNNING;

	switch(vm.count("lst"))
	{
	case 0: break;
	case 1:
		std::cout << "--lst option not supported yet\n";
		break;

	default:
		assert(0 && "too many lst options");
		break;
	}
}

void ServiceImpl::setState(SimState::type state)
{
	{
		boost::lock_guard<boost::mutex> lock(simStateMutex);
		prevSimState = simState;
		simState = state;
	}

	simStateCond.notify_all();
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

	//memory
	ramHandle = findVerilogModule(top, "ram", "mem", NULL);
	
	microStepHandle = vpi_handle_by_name("micro_steps", top);
	assert(microStepHandle);
}

void ServiceImpl::serverThreadProc()
{
	shared_ptr<ServiceImpl> handler(this);
	shared_ptr<TProcessor> processor(new SimServiceProcessor(handler));
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	TNonblockingServer server(processor, protocolFactory, PORT);

	server.serve();
}

void ServiceImpl::setMicroStep()
{
	s_vpi_value val;
	val.format = vpiIntVal;
	val.value.integer = 1;
	vpi_put_value(microStepHandle, &val, NULL, vpiNoDelay);
}

static void registerTasks()
{
	s_vpi_systf_data systf;
	systf.type = vpiSysTask;
	systf.compiletf = NULL;
	systf.sizetf    = NULL;
	systf.user_data = reinterpret_cast<PLI_BYTE8*>(new ServiceImpl());

	for(const VerilogTask* t = VERILOG_TASKS; t < VERILOG_TASKS + VERILOG_TASKS_SIZE; ++t)
	{
		systf.tfname    = t->name;
		systf.calltf    = t->calltf;
		vpi_register_systf(&systf);
	}
}
