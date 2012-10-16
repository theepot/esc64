`include "ir.v"

//module instructionRegister(clock, notReset, notLoad, OE, in, outOpcode, outOp0, outOp1, outOp2);

module ir_test();
	reg [15:0] data;
	reg dataE;
	wire [6:0] opcode;
	wire [2:0] op0, op1, op2;
	reg clock, notReset, notLoad, OE;
	
	assign data_bus = dataE ? data : 16'bzzzzzzzzzzzzzzzz;
	
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0, data, opcode, op0, op1, op2, clock, notReset, notLoad, OE);
	
		dataE = 0;
		clock = 0;
		notReset = 1;
		notLoad = 1;
		OE = 1;
		data = 0;
		
		#20	OE = 1;
		#20	OE = 0;
		
		#20 data = 16'HF0F0;
		#20	dataE = 1;
		#20 notLoad = 0;
		#20 notLoad = 1;
		#20	dataE = 0;
		
		#20	OE = 1;
		#20	OE = 0;
		
		#50 $finish;
	end
	
	always begin
		#5 clock = ~clock;
	end
	
	instructionRegister ir(clock, notReset, notLoad, OE, data, opcode, op0, op1, op2);

endmodule
