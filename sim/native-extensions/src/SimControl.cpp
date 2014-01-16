#include "service/SimService.h"
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

	void parseOptions(int argc, char** argv);
	void setState(SimState::type state);
	void initModuleHandles();
	void serverThreadProc();
};

struct VerilogTask
{
	const char* name;
	void (ServiceImpl::*func)();
};

static const VerilogTask VERILOG_TASKS[] =
{
	{ "$tick", &ServiceImpl::tickTask, },
	{ "$print_add", &ServiceImpl::printNumTask },
	{ "$start_thrift_server", &ServiceImpl::startServerTask },
	{ "$halt", &ServiceImpl::haltTask },
	{ "$error", &ServiceImpl::errorTask }
};

static const size_t VERILOG_TASKS_SIZE = sizeof VERILOG_TASKS / sizeof (VerilogTask);

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
	setState(SimState::STARTING_STEP);

	boost::unique_lock<boost::mutex> lock(simStateMutex);
	while(simState != SimState::PAUSED)
	{
		simStateCond.wait(lock);
	}
}

void ServiceImpl::microStep()
{
	setState(SimState::MICRO_STEPPING);

	boost::unique_lock<boost::mutex> lock(simStateMutex);
	while(simState == SimState::MICRO_STEPPING)
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
//	unsigned off = offset;
//	unsigned sz = size;
//
//	if(off > 0xFFFF || off + sz > 0x10000)
//	{
//		return;
//	}
//
//	boost::lock_guard<boost::mutex> lock(simMutex);
//
//	s_vpi_value val;
//	val.format = vpiIntVal;
//
//	for(unsigned i = off; i < off + sz; ++i)
//	{
//		vpiHandle h = vpi_handle_by_index(ramHandle, i);
//		assert(h);
//		vpi_get_value(h, &val);
//		_return.push_back(val.value.integer);
//	}
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
			/*FIXME debug*/ //std::cout << "tickTask(): state=" << _SimState_VALUES_TO_NAMES.at(s) << std::endl;
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

		case SimState::STARTING_STEP:
			setState(SimState::STEPPING);
			return;

		case SimState::STEPPING:
		{
			ArgumentIterator args;
			PLI_INT32 fetch = args.NextInt();
			if(fetch)
			{
				setState(SimState::PAUSED);
				break;
			}
		} return;
			
		case SimState::MICRO_STEPPING:
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
	po::options_description desc( "Allowed options");

	desc.add_options()
	    ("paused", "start the simulation paused")
	    ("lst", po::value<std::string>(), "program lst file");

	boost::program_options::basic_command_line_parser<char> bcp = boost::program_options::basic_command_line_parser<char>(argc, argv);
	bcp.options(desc);
	bcp.allow_unregistered();

	po::variables_map vm;
	po::store(bcp.run(), vm);
	po::notify(vm);

	prevSimState = simState = (vm.count("paused") ? SimState::HALTED : SimState::RUNNING);
	std::cout << "Starting simulation in " << _SimState_VALUES_TO_NAMES.at(simState) << " state\n";

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
}

void ServiceImpl::serverThreadProc()
{
	shared_ptr<ServiceImpl> handler(this);
	shared_ptr<TProcessor> processor(new SimServiceProcessor(handler));
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	TNonblockingServer server(processor, protocolFactory, PORT);

	server.serve();
}

void SimControl_entry(void)
{
	ServiceImpl* s = new ServiceImpl();

	for(const VerilogTask* t = VERILOG_TASKS; t < VERILOG_TASKS + VERILOG_TASKS_SIZE; ++t)
	{
		registerSysTF(t->name, boost::bind(t->func, s), vpiSysTask);
	}
}
