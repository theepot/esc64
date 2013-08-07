require 'thrift'
require 'sim_service'

module Sim
	class Watch
		attr_reader :address
		attr_reader :size
	
		def initialize address, size
			@address = address
			@size = size
		end
	end

	class Client
		DEC = lambda { |num| sprintf "%05i", num }
		HEX = lambda { |num| sprintf "%04X", num }
		BIN = lambda { |num| s = num.to_s 2; "#{'0' * (16 - s.size)}#{s}" }
	
		attr_reader		:client
		attr_reader		:watches
		attr_accessor	:port
		attr_accessor	:hostname
		
		def initialize hostname, port
			@hostname = hostname
			@port = port
			@client = nil
			@transport = nil
			@watches = {}
		end
		
		def disconnect
			if @transport
				@transport.close
			end
		end
	
		def connect
			@transport = Thrift::FramedTransport.new(Thrift::Socket.new(@hostname, @port))
			protocol = Thrift::BinaryProtocol.new(@transport)
			@client = SimService::Client.new(protocol)
			@transport.open
		end
	
		def reconnect
			disconnect
			connect
		end
		
		def p_membin offset, amount
			mem = @client.getMemory offset, amount
			for i in offset...(offset + amount)
				s = mem[i].to_s 2
				puts "\t#{sprintf '0x%04X', i} : 0b#{'0' * (16 - s.size)}#{s}"
			end
		end
	
		def p_regs
			regNames = [ "r0   ", "r1   ", "r2   ", "r3   ", "r4   ", "r5/sp", "r6/lr", "r7/pc" ]
			regs = @client.getRegister 0, 8
			for i in 0...regs.size
				puts "\t#{regNames[i]} : #{regs[i]}"
			end
		end
		
		
		def dump_lst file, offset, amount
			mem = @client.getMemory offset, amount
			File.open(file, "w") do |f|
				for v in mem
					s = v.to_s 2
					f.write "#{'0' * (16 - s.size)}#{s}\n"
				end
			end
			return nil
		end
		
		def add_watch name, address, size
			@watches[name] = Watch.new address, size
		end
		
		def p_watches
			for w in @watches
				puts "\t#{w[0]}:"
				p_membin w[1].address, w[1].size
				puts ""
			end
		end
	end

	begin
		@client = Client.new "localhost", 9090
		@client.connect
	end
	
	def self.client; @client end
	def self.client=c; @client = c end
end
