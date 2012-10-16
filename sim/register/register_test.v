`include "register.v"
module register_test();
	wire [15:0] data_bus;
	reg [15:0] data;
	reg clock, notReset, notLoad, regOE;
	reg oe;
	
	assign data_bus = oe ? data : 16'bzzzzzzzzzzzzzzzz;
	
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0, data_bus, data, clock, notReset, notLoad, regOE, oe);
	
		clock = 0;
		notReset = 1;
		regOE = 0;
		notLoad = 1;
		data = 0;
		oe = 0;
		
		#20 notReset = 0;
		#20 notReset = 1;
		#5 regOE = 1;

		#20 regOE = 0;
		
		#10 data = 16'HF0F0;
		oe = 1;
		#10 notLoad = 0;
		#20 notLoad = 1;
		oe = 0;
		#10 regOE = 1;
		
		#8 notReset = 0;
		
		#10 $finish;
	end
	
	always begin
		#5 clock = ~clock;
	end
	
	//register(clock, notReset, notLoad, OE, in, out);
	register #(.DATA_WIDTH(16)) register(clock, notReset, notLoad, regOE, data_bus, data_bus);

endmodule
