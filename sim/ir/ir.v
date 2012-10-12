`ifndef _IR_INCLUDED_
`define _IR_INCLUDED_

`include "../register/register.v"

module instructionRegister(clock, OE, load, in, outOpcode, outOp0, outOp1, outOp2);
	input clock, OE, load;
	input [15:0] in;
	
	wire [15:0] in;
	
	output [6:0] outOpcode;
	output [2:0] outOp0, outOp1, outOp2;

	wire [15:0] regOut;
		
	wire [6:0] outOpcode;
	wire [2:0] outOp0, outOp1, outOp2;
	
	register ir(clock, 0, OE, load, in, regOut);
	
	assign outOpcode = regOut[15:9];
	assign outOp0 = regOut[8:6];
	assign outOp1 = regOut[5:3];
	assign outOp2 = regOut[2:0];
	
endmodule

`endif
