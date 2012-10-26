`ifndef _IR_S_INCLUDED_
`define _IR_S_INCLUDED_

`include "../74xxx/octRegister_74377.v"

/* Instruction format
   15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
  | opcode             | op0    | op1    | op2    |
   07 06 05 04 03 02 01 00|07 06 05 04 03 02 01 00
                        H | L
*/

module instructionRegister(clock, notLoad, in, outOpcode, outOp0, outOp1, outOp2);
	input clock, notReset, notLoad;
	
	input [15:0] in;
	wire [15:0] in;
	
	output [6:0] outOpcode;
	wire [6:0] outOpcode;
	
	output [2:0] outOp0, outOp1, outOp2;
	wire [2:0] outOp0, outOp1, outOp2;
	
	wire [16:0] regOut;
	
	octRegister_74377 regH(clock, notLoad, in[15:8], regOut[15:8]);
	octRegister_74377 regL(clock, notLoad, in[7:0], regOut[7:0]);
	
	assign outOpcode = regOut[15:9];
	assign outOp0 = regOut[8:6];
	assign outOp1 = regOut[5:3];
	assign outOp2 = regOut[2:0];
	
endmodule

`endif
