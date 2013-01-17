`include "register.v"
`include "globals.v"

module register_test();
	wire [15:0] data_bus;
	reg [15:0] data;
	reg clock, notReset, notLoad, regOE;
	reg oe;
	
	assign data_bus = oe ? data : 16'bzzzzzzzzzzzzzzzz;
	
	initial begin
		$dumpfile("test.vcd");
		$dumpvars(0, data_bus, data, clock, notReset, notLoad, regOE, oe);
	
		clock = 0;
		notReset = 1;
		regOE = 0;
		notLoad = 1;
		data = 0;
		oe = 0;
		
		# `TEST_DELAY notReset = 0;
		# `TEST_DELAY notReset = 1;
		
		# `TEST_DELAY if(data_bus !== 16'bzzzzzzzzzzzzzzzz) begin
			$display("ERROR:register. Got %X on outputs. Expected 0xZZZZ.", data_bus);
		end
		
		# `TEST_DELAY regOE = 1;
		# `TEST_DELAY if(data_bus !== 16'H0000) begin
			$display("ERROR:register. Got %X on outputs. Expected 0x0000.", data_bus);
		end
		
		# `TEST_DELAY oe = 1;
		data = 16'HBEEF;
		regOE = 0;
		notLoad = 0;
		
		# `TEST_DELAY notLoad = 1;
		regOE = 1;
		oe = 0;
		
		# `TEST_DELAY if(data_bus !== 16'HBEEF) begin
			$display("ERROR:register. Got %X on outputs. Expected 0xBEEF.", data_bus);
		end
		
		# `TEST_DELAY notReset = 0;
		# `TEST_DELAY notReset = 1;
		
		# `TEST_DELAY if(data_bus !== 16'H0000) begin
			$display("ERROR:register. Got %X on outputs. Expected 0x0000.", data_bus);
		end
		
		#10 $finish;
	end
	
	always begin
		# `TEST_CLOCK clock = ~clock;
	end
	
	//register(clock, notReset, notLoad, OE, in, out);
	register #(.DATA_WIDTH(16)) register(clock, notReset, notLoad, regOE, data_bus, data_bus);

endmodule
