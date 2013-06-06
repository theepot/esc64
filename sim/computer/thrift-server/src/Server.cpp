#include "service/SimService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
//#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/thread.hpp>
#include <sstream>

extern "C"
{
#include <vpi_user.h>
}

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace ::esc64simsrv;

///// thrift server /////
static const int PORT = 9090;
static boost::thread* serviceThread = NULL;

class ServiceImpl : virtual public SimServiceIf
{
public:
	ServiceImpl() { }
	SimState::type getState();
	void start();
	void stop();
	void pause();
	void step();
	void getRegister(std::vector<int16_t> & _return, const int32_t offset, const int32_t size);
	void getMemory(std::vector<int16_t> & _return, const int32_t offset, const int32_t size);
};

static void ServerThreadProc();

///// simulation handles /////
//TODO registers
//TODO memory

///// verilog tasks /////
static int CompileTfDummy(char* user_data);
static int TickTask(char* unused);
static int PrintNumTask(char* unused);
static int StartServerTask(char* unused);
static int SetHandlesTask(char* unused);
static void RegisterTasks();

static const struct VerilogTask_
{
	const char* name;
	PLI_INT32 (*calltf)(PLI_BYTE8*);
} VERILOG_TASKS[] =
{
	{ "$tick", TickTask },
	{ "$print_add", PrintNumTask },
	{ "$start_thrift_server", StartServerTask },
	{ "$set_handles", SetHandlesTask }
};

static const size_t VERILOG_TASKS_SIZE = sizeof VERILOG_TASKS / sizeof (VerilogTask_);
void (*vlog_startup_routines[])() = { RegisterTasks, 0 };


///// verilog utilities /////
struct VpiIterator
{
	vpiHandle iterator;

	VpiIterator(PLI_INT32 type, vpiHandle ref);
	~VpiIterator();
	vpiHandle Next();
};

struct ArgumentIterator
{
	struct t_vpi_value argValue;
	vpiHandle argHandle;
	VpiIterator* it;

	ArgumentIterator(VpiIterator* it);
	bool Next(PLI_INT32 format);
};

///// implementation /////
SimState::type ServiceImpl::getState()
{
	return SimState::STOPPED;
}

void ServiceImpl::start()
{
	//TODO
}

void ServiceImpl::stop()
{
	//TODO
}

void ServiceImpl::pause()
{
	//TODO
}

void ServiceImpl::step()
{
	//TODO
}

void ServiceImpl::getRegister(std::vector<int16_t> & _return, const int32_t offset, const int32_t size)
{
	//TODO
}

void ServiceImpl::getMemory(std::vector<int16_t> & _return, const int32_t offset, const int32_t size)
{
	//TODO
}

static void ServerThreadProc()
{
	//TODO use TNonBlockingServer
	shared_ptr<ServiceImpl> handler(new ServiceImpl());
	shared_ptr<TProcessor> processor(new SimServiceProcessor(handler));
	shared_ptr<TServerTransport> serverTransport(new TServerSocket(PORT));
	shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);

	server.serve();
}

static void RegisterTasks()
{
	for(size_t i = 0; i < VERILOG_TASKS_SIZE; ++i)
	{
		const struct VerilogTask_* task = &VERILOG_TASKS[i];
		s_vpi_systf_data tfTick;

		tfTick.type      = vpiSysTask;
		tfTick.tfname    = task->name;
		tfTick.calltf    = task->calltf;
		tfTick.compiletf = CompileTfDummy;
		tfTick.sizetf    = 0;
		tfTick.user_data = 0;

		vpi_register_systf(&tfTick);
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

ArgumentIterator::ArgumentIterator(VpiIterator* it) :
		argValue(),
		argHandle(NULL),
		it(it)
{
}

bool ArgumentIterator::Next(PLI_INT32 format)
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

static int CompileTfDummy(char* user_data)
{
	return 0; //does nothing
}

static int TickTask(char* unused)
{
	//TODO
	return 0;
}

static int PrintNumTask(char* unused)
{
	vpiHandle systf = vpi_handle(vpiSysTfCall, NULL);
	VpiIterator it(vpiArgument, systf);
	ArgumentIterator argIt(&it);

	assert(argIt.Next(vpiIntVal));
	PLI_INT32 a = argIt.argValue.value.integer;
	assert(argIt.Next(vpiIntVal));
	PLI_INT32 b = argIt.argValue.value.integer;

	vpi_printf("a(%d) + b(%d) = %d\n", a, b, a + b);

	return 0;
}

static int StartServerTask(char* unused)
{
	assert(serviceThread == NULL);
	serviceThread = new boost::thread(ServerThreadProc);
	return 0;
}

static int SetHandlesTask(char* unused)
{
	//TODO
	return 0;
}
