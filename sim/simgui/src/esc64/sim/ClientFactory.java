package esc64.sim;

import org.apache.thrift.TException;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.transport.TTransportException;

import esc64.sim.srv.SimService;
import esc64.sim.srv.SimState;

public class ClientFactory
{
	private static final String HOSTNAME = "localhost";
	private static final int PORT = 9090;
	
	public SimService.Client newClient() throws TTransportException
	{
		TTransport transport = new TSocket(HOSTNAME, PORT);
		transport.open();
		TProtocol protocol = new TBinaryProtocol(transport);
		SimService.Client client = new SimService.Client(protocol);
		
		return client;
	}
	
	public static void main(String[] args)
	{
		try
		{
			TTransport transport = new TSocket("localhost", PORT);
			transport.open();
			TProtocol protocol = new TBinaryProtocol(transport);
			SimService.Client client = new SimService.Client(protocol);
		
			SimState state = client.getState();
			System.out.println("state=" + state);
		}
		catch (TException e)
		{
			e.printStackTrace();
			System.exit(1);
		}
	}
}
