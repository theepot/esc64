require 'thrift'
require 'sim_service'

module Sim
	begin
		PORT = 9090
		HOSTNAME = "localhost"
	
		transport = Thrift::BufferedTransport.new(Thrift::Socket.new(HOSTNAME, PORT))
		protocol = Thrift::BinaryProtocol.new(transport)
		@client_ = SimService::Client.new(protocol)

		transport.open()	
	rescue
		abort $!.message
	end
	
	def self.client
		return @client_
	end
end
