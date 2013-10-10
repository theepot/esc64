#include "service/SimService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/thread.hpp>
#include <sstream>

extern "C"
{
#include <vpi_user.h>
}

#define BIND_TASK(a, b)	static int a(PLI_BYTE8* p) { reinterpret_cast<ServiceImpl*>(p)->b(); return 0; }

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace ::esc64simsrv;

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
	ErrCode::type getErrCode();
	void reset();
	void getRegister(std::vector<int32_t> & _return, const int32_t offset, const int32_t size);
	void getMemory(std::vector<int32_t> & _return, const int32_t offset, const int32_t size);

	//verilog tasks
	void tickTask();
	void printNumTask();
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

	void setState(SimState::type state);
	void initModuleHandles();
	void serverThreadProc();
};

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

///// verilog utilities /////
struct VpiIterator
{
	vpiHandle iterator;

	VpiIterator(PLI_INT32 type, vpiHandle ref);
	~VpiIterator();
	vpiHandle Next();
};

/**
 * @param ...	NULL-terminated list of const char*
 */
static vpiHandle findVerilogModule(vpiHandle handle, ...);

struct ArgumentIterator
{
	struct t_vpi_value argValue;
	vpiHandle argHandle;
	VpiIterator* it;

	ArgumentIterator(VpiIterator* it);
	bool TryNext(PLI_INT32 format);
	void Next(PLI_INT32 format);
};

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

ErrCode::type ServiceImpl::getErrCode()
{
	boost::lock_guard<boost::mutex> lock(simMutex);
	return simError;
}

void ServiceImpl::reset()
{
	boost::lock_guard<boost::mutex> lock(simMutex);
	//TODO
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

		default:
			throw std::runtime_error("Illegal simulation state");
		}
	}
}

void ServiceImpl::printNumTask()
{
	vpiHandle systf = vpi_handle(vpiSysTfCall, NULL);
	VpiIterator it(vpiArgument, systf);
	ArgumentIterator argIt(&it);

	assert(argIt.TryNext(vpiIntVal));
	PLI_INT32 a = argIt.argValue.value.integer;
	assert(argIt.TryNext(vpiIntVal));
	PLI_INT32 b = argIt.argValue.value.integer;

	vpi_printf("a(%d) + b(%d) = %d\n", a, b, a + b);
}

void ServiceImpl::startServerTask()
{
	assert(serviceThread == NULL);

	//get module handles
	initModuleHandles();

	//initialize simulation state / sync
	prevSimState = simState = SimState::HALTED;
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

	vpiHandle systf = vpi_handle(vpiSysTfCall, NULL);
	VpiIterator it(vpiArgument, systf);
	ArgumentIterator argIt(&it);

	if(!argIt.TryNext(vpiIntVal))
	{
		throw std::runtime_error("No error value passed");
	}

	PLI_INT32 e = argIt.argValue.value.integer;
	simError = static_cast<ErrCode::type>(e);
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
	//TODO

	ramHandle = findVerilogModule(top, "ram", "mem", NULL);
}

void ServiceImpl::serverThreadProc()
{
	shared_ptr<ServiceImpl> handler(this);
	shared_ptr<TProcessor> processor(new SimServiceProcessor(handler));
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	TNonblockingServer server(processor, protocolFactory, PORT);

	server.serve();
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

VpiIterator::VpiIterator(PLI_INT32 type, vpiHandle ref) :
		iterator(NULL)
{
	iterator = vpi_iterate(type, ref);
	assert(iterator);
}

VpiIterator::~VpiIterator()
{
	if(iterator != NULL)
	{
		vpi_free_object(iterator);
	}
}

vpiHandle VpiIterator::Next()
{
	return vpi_scan(iterator);
}

static vpiHandle findVerilogModule(vpiHandle handle, ...)
{
	va_list vlist;
	va_start(vlist, handle);

	const char* arg;
	while((arg = va_arg(vlist, const char *)))
	{
		handle = vpi_handle_by_name(arg, handle);
		if(!handle)
		{
			va_end(vlist);
			std::stringstream ss;
			ss << "Unable to find `" << arg << "'\n";
			throw std::runtime_error(ss.str());
		}
	}

	va_end(vlist);
	return handle;
}

ArgumentIterator::ArgumentIterator(VpiIterator* it) :
		argValue(),
		argHandle(NULL),
		it(it)
{
}

bool ArgumentIterator::TryNext(PLI_INT32 format)
{
	argValue.format = format;
	argHandle = it->Next();
	if(argHandle != NULL)
	{
		vpi_get_value(argHandle, &argValue);
		return true;
	}

	return false;
}

void ArgumentIterator::Next(PLI_INT32 format)
{
	if(!TryNext(format))
	{
		throw std::runtime_error("Unable to get next argument");
	}
}
