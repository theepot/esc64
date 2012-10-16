`include "ir.v"

module ir_test();
	reg [15:0] data;
	wire [6:0] opcode;
	wire [2:0] op0, op1, op2;
	reg clock, output_enable, load;
	
	assign data_bus = oe ? data : 16'bzzzzzzzzzzzzzzzz;
	
	
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0, clock, output_enable, load, data, opcode, op0, op1, op2);
	
		clock = 0;
		output_enable = 0;
		load = 0;
		data = 0;
		
		#5 output_enable = 1;

		#20 output_enable = 0;
		
		#10 data = 16'HF0F0;
		#10 load = 1;
		#20 load = 0;
		#10 output_enable = 1;
		
		#50 $finish;
	end
	
	always begin
		#5 clock = ~clock;
	end
	
	instructionRegister ir(clock, output_enable, load, data, opcode, op0, op1, op2);

endmodule
