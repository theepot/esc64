`include "register.v"
module register_test();
	wire [15:0] data_bus;
	reg [15:0] data;
	reg clock, reset, output_enable, load;
	reg oe;
	
	assign data_bus = oe ? data : 16'bzzzzzzzzzzzzzzzz;
	
	
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0, clock, reset, output_enable, load, data, data_bus);
	
		clock = 0;
		reset = 0;
		output_enable = 0;
		load = 0;
		data = 0;
		oe = 0;
		
		#20 reset = 1;
		#20 reset = 0;
		#5 output_enable = 1;

		#20 output_enable = 0;
		
		#10 data = 16'HF0F0;
		oe = 1;
		#10 load = 1;
		#20 load = 0;
		oe = 0;
		#10 output_enable = 1;
		
		#8 reset = 1;
		
		#10 $finish;
	end
	
	always begin
		#5 clock = ~clock;
	end
	
	register #(.DATA_WIDTH(16)) register(clock, reset, output_enable, load, data_bus, data_bus);

endmodule
