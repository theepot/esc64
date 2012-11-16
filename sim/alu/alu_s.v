`ifndef _ALU_S_INCLUDED_
`define _ALU_S_INCLUDED_

`include "../74xxx/ALU_74181.v"
`include "../74xxx/OctThreeState_74541.v"
`include "../74xxx/OctInputOrNor_744078.v"
`include "../74xxx/Quad2To1Mux_74157.v"
`include "../74xxx/Quad2InputAnd_7408.v"
`include "../74xxx/Dual4To1Mux_74153.v"
`include "../74xxx/HexInverter_7404.v"

`include "../alu/alu_common.v"

module alu(a, b, y, f, csel, ucin, fcin, cout, zout, notALUOE, notShiftOE);
	input   [15:0] a;
	input   [15:0] b;
	output  [15:0] y;
	input   [4:0]  f;
	input   fsel, csel, ucin, fcin, notALUOE, notShiftOE;
	output	cout, zout;
	
	//carry in selection
	wire [3:0] cinMuxOut;
	wire aluCin;
	assign aluCin = cinMuxOut[3];
	Quad2To1Mux_74157 cinMux(0, csel, {ucin,3'bxxx}, {fcin,3'bxxx}, cinMuxOut);
	
	//carry out selection
	wire coutLeft, coutRight;
	wire [1:0] coutMuxOut;
	Dual4To1Mux_74153 coutMux(0, {f[0], notShiftOE}, {coutRight, 1'bx}, {alu3cout, 1'bx}, {coutLeft, 1'bx}, {alu3cout, 1'bx}, coutMuxOut);
	assign cout = coutMuxOut[1];
	
	
	//inverting some signals
	wire alu3cout;
	wire aluNotCin;
	wire [5:0] miscInverterOut;
	wire f0Inverted;
	assign f0Inverted = miscInverterOut[3];
	assign alu3cout = miscInverterOut[4];
	assign aluNotCin = miscInverterOut[5];
	HexInverter_7404 miscInverter({aluCin, alu3NotCout, f[0], 3'bxxx}, miscInverterOut);
	
	//74181 alu
	wire [15:0] aluOutInt;
	wire alu0NotCout, alu1NotCout, alu2NotCout, alu3NotCout;
	ALU_74181 alu0(f[4:1],   a[3:0],   b[3:0],   f[0], aluNotCin,   aluOutInt[3:0],   /*NC*/, /*NC*/, alu0NotCout, /*NC*/); //LS
	ALU_74181 alu1(f[4:1],   a[7:4],   b[7:4],   f[0], alu0NotCout, aluOutInt[7:4],   /*NC*/, /*NC*/, alu1NotCout, /*NC*/);
	ALU_74181 alu2(f[4:1],   a[11:8],  b[11:8],  f[0], alu1NotCout, aluOutInt[11:8],  /*NC*/, /*NC*/, alu2NotCout, /*NC*/);
	ALU_74181 alu3(f[4:1],   a[15:12], b[15:12], f[0], alu2NotCout, aluOutInt[15:12], /*NC*/, /*NC*/, alu3NotCout, /*NC*/); //MS
	
	OctThreeState_74541 aluOutputBuffer0(notALUOE, 0, aluOutInt[7:0],  y[7:0]); //LS
	OctThreeState_74541 aluOutputBuffer1(notALUOE, 0, aluOutInt[15:8], y[15:8]); //MS
	
	//shift
	wire [15:0] aShiftedRight;
	assign aShiftedRight = {1'b0, a[15:1]};
	assign coutRight = a[0];
	wire [15:0] aShiftedLeft;
	assign aShiftedLeft = {a[14:0], 1'b0};
	assign coutLeft = a[15];
	OctThreeState_74541 shiftLeftOutputBuffer0(notShiftOE, f0Inverted, aShiftedLeft[7:0],  y[7:0]); //LS
	OctThreeState_74541 shiftLeftOutputBuffer1(notShiftOE, f0Inverted, aShiftedLeft[15:8], y[15:8]); //MS
	OctThreeState_74541 shiftRightOutputBuffer0(notShiftOE, f[0], aShiftedRight[7:0],  y[7:0]); //LS
	OctThreeState_74541 shiftRightOutputBuffer1(notShiftOE, f[0], aShiftedRight[15:8], y[15:8]); //MS

	//zero flag
	wire notZero0Out, notZero1Out;
	OctInputOrNor_744078 zero0(y[7:0],  /*NC*/, notZero0Out); //LS
	OctInputOrNor_744078 zero1(y[15:8], /*NC*/, notZero1Out); //MS
	wire [3:0] notZeroesAndOut;
	assign zout = notZeroesAndOut[3];
	Quad2InputAnd_7408 notZeroesAnd({notZero0Out,3'b000}, {notZero1Out,3'b000}, notZeroesAndOut);
	
	
	endmodule

`endif

