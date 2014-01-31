`ifndef _ALU_S_INCLUDED_
`define _ALU_S_INCLUDED_

`include "ALU_74181.v"
`include "OctThreeState_74541.v"
`include "OctInputOrNor_744078.v"
`include "Quad2To1Mux_74157.v"
`include "Quad2InputAnd_7408.v"
`include "Dual4To1Mux_74153.v"
`include "HexInverter_7404.v"
`include "Quad2InputXor_7486.v"

`define ALU_F_A				5'b00000
`define ALU_F_B				5'b10101
`define ALU_F_SUB			5'b01100
`define ALU_F_ADD			5'b10010
`define ALU_F_NOT			5'b00001
`define ALU_F_XOR			5'b01101
`define ALU_F_AND			5'b10111
`define ALU_F_OR			5'b11101
`define ALU_F_A_MINUS_ONE	5'b11110
`define ALU_F_ZERO			5'b00111

`define ALU_F_SHIFT_LEFT	5'b00101
`define ALU_F_SHIFT_RIGHT	5'b00010
`define ALU_CSEL_UCIN		0
`define ALU_CSEL_SRCIN		1

module alu(a, b, y, f, csel, ucin, srcin, cout, zout, signed_cmp, notALUOE, notExtraOE);
	input   [15:0] a;
	input   [15:0] b;
	output  [15:0] y;
	input   [4:0]  f;
	input   fsel, csel, ucin, srcin, signed_cmp, notALUOE, notExtraOE;
	output	cout, zout;
	
	//carry out selection
	wire coutSignExtended, coutRight;
	wire [1:0] coutMuxOut;
	Dual4To1Mux_74153 coutMux(0, {f[2], notExtraOE}, {coutRight, 1'bx}, {alu3cout, 1'bx}, {coutSignExtended, 1'bx}, {alu3cout, 1'bx}, coutMuxOut);

	
	//inverting some signals
	wire alu3cout;
	wire not_ucin_and_not_csel;
	wire not_srcin_and_csel;
	wire not_csel;
	wire [5:0] miscInverterOut;
	assign not_csel = miscInverterOut[2];
	assign not_srcin_and_csel = miscInverterOut[3];
	assign alu3cout = miscInverterOut[4];
	assign not_ucin_and_not_csel = miscInverterOut[5];
	HexInverter_7404 miscInverter({ucin_and_not_csel, alu3NotCout, srcin_and_csel, csel, 2'bxx}, miscInverterOut);
	
	//74181 alu
	wire [15:0] aluOutInt;
	wire alu0NotCout, alu1NotCout, alu2NotCout, alu3NotCout;
	ALU_74181 alu0(f[4:1],   a[3:0],   b[3:0],   f[0], aluNotCin,   aluOutInt[3:0],   /*NC*/, /*NC*/, alu0NotCout, /*NC*/); //LS
	ALU_74181 alu1(f[4:1],   a[7:4],   b[7:4],   f[0], alu0NotCout, aluOutInt[7:4],   /*NC*/, /*NC*/, alu1NotCout, /*NC*/);
	ALU_74181 alu2(f[4:1],   a[11:8],  b[11:8],  f[0], alu1NotCout, aluOutInt[11:8],  /*NC*/, /*NC*/, alu2NotCout, /*NC*/);
	ALU_74181 alu3(f[4:1],   a[15:12], b[15:12], f[0], alu2NotCout, aluOutInt[15:12], /*NC*/, /*NC*/, alu3NotCout, /*NC*/); //MS

	OctThreeState_74541 aluOutputBuffer0(notALUOE, 0, aluOutInt[7:0],  y[7:0]); //LS
	OctThreeState_74541 aluOutputBuffer1(notALUOE, 0, aluOutInt[15:8], y[15:8]); //MS
	
	//extra
	wire [15:0] aShiftedRight;
	assign aShiftedRight = {1'b0, a[15:1]};
	assign coutRight = a[0];
	wire [15:0] aSignExtended;
	assign aSignExtended = {{8{a[7]}}, a[7:0]};
	assign coutSignExtended = a[7];
	OctThreeState_74541 signExtendedOutputBuffer0(notExtraOE, f[1], aSignExtended[7:0],  y[7:0]); //LS
	OctThreeState_74541 signExtendedOutputBuffer1(notExtraOE, f[1], aSignExtended[15:8], y[15:8]); //MS
	OctThreeState_74541 shiftRightOutputBuffer0(notExtraOE, f[0], aShiftedRight[7:0],  y[7:0]); //LS
	OctThreeState_74541 shiftRightOutputBuffer1(notExtraOE, f[0], aShiftedRight[15:8], y[15:8]); //MS

	//zero flag
	wire notZero0Out, notZero1Out;
	OctInputOrNor_744078 zero0(y[7:0],  /*NC*/, notZero0Out); //LS
	OctInputOrNor_744078 zero1(y[15:8], /*NC*/, notZero1Out); //MS
	
	//and0
	wire [3:0] and0_out;
	assign zout = and0_out[3];
	
	wire srcin_and_csel;
	assign srcin_and_csel = and0_out[2];
	
	wire ucin_and_not_csel;
	assign ucin_and_not_csel = and0_out[1];
	
	wire aluNotCin;
	assign aluNotCin = and0_out[0];
	Quad2InputAnd_7408 and0({notZero0Out,srcin,ucin,not_ucin_and_not_csel}, {notZero1Out,csel,not_csel,not_srcin_and_csel}, and0_out);
	
	//signed compare carry fix
	wire [3:0] xor0_out;
	assign cout = xor0_out[0];
	Quad2InputXor_7486 xor0({2'b00, and1_out[0], xor0_out[1]}, {2'b00, and1_out[1], coutMuxOut[1]}, xor0_out);
	
	wire [3:0] and1_out;
	Quad2InputAnd_7408 and1({2'b00, signed_cmp, signed_cmp}, {2'b00, a[15], b[15]}, and1_out);
	

	
	endmodule

`endif

