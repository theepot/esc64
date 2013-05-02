#include "hello-gen/HelloService.h"
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

using namespace ::Hello;

static const int PORT = 9090;

class HelloServiceHandler : virtual public HelloServiceIf
{
public:
	HelloServiceHandler() { }
	void sayHello(const std::string& name);
	void getHello(std::string& result, const std::string& name);
};

static void ServerThreadProc();
static int TickCompile(char*user_data);
static int TickCall(char*user_data);
static void TickRegister();

void (*vlog_startup_routines[])() =
{
	TickRegister
};

int main(int argc, char** argv)
{
	std::cout << "Starting server thread..." << std::endl;
	boost::thread serverThread(ServerThreadProc);

	std::cout << "Joining server thread..." << std::endl;
	serverThread.join();

	std::cout << "Exit" << std::endl;
	return 0;
}

void HelloServiceHandler::sayHello(const std::string& name)
{
	std::cout << "Hello " << name << "!" << std::endl;
}

void HelloServiceHandler::getHello(std::string& result, const std::string& name)
{
	std::stringstream ss;
	ss << "Hello " << name << "!";
	result = ss.str();
}

static void ServerThreadProc()
{
	shared_ptr<HelloServiceHandler> handler(new HelloServiceHandler());
	shared_ptr<TProcessor> processor(new HelloServiceProcessor(handler));
	shared_ptr<TServerTransport> serverTransport(new TServerSocket(PORT));
	shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
	TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);

	server.serve();
}

void TickRegister()
{
      s_vpi_systf_data tfTick;

      tfTick.type      = vpiSysTask;
      tfTick.tfname    = "$tick";
      tfTick.calltf    = TickCall;
      tfTick.compiletf = TickCompile;
      tfTick.sizetf    = 0;
      tfTick.user_data = 0;
      vpi_register_systf(&tfTick);
      
      boost::thread serverThread(ServerThreadProc);
}


static int TickCompile(char* unused)
{
	return 0;
}

static int TickCall(char* unused)
{
	return 0;
}

