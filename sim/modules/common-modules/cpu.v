`ifdef STATUS_STRUCT
`include "../status/status_s.v"
`else
`include "../status/status.v"
`endif

`ifdef IR_STRUCT
`include "../ir/ir_s.v"
`else
`include "../ir/ir.v"
`endif

`ifdef ALU_STRUCT
`include "../alu/alu_s.v"
`else
`include "../alu/alu.v"
`endif

`ifdef REGSEL_STRUCT
`include "../regsel/regsel_s.v"
`else
`include "../regsel/regsel.v"
`endif

`ifdef BREG_STRUCT
`include "../breg/breg_s.v"
`else
`include "../breg/breg.v"
`endif

`ifdef MSEQ_STRUCT
`include "../mSeq/mSeq_s.v"
`else
`include "../mSeq/mSeq.v"
`endif

`include "../regFile/regFile.v"
`include "../rwRegister/rwRegister_s.v"//TODO: include right version depending on simulation level

module cpu(clock, notReset, aBus, yBus, memNotRead, memNotWrite);
	input clock, notReset;
	output memNotRead, memNotWrite;
	inout [15:0] aBus, yBus;
	
	wire clock, notReset, memNotRead, memNotWrite;
	
	//busses
	wire [15:0] aBus;
	wire [15:0] yBus;

	//register selection
	wire regselOE; //regsel < useq
	wire regselLoad; //regsel < useq
	wire [1:0] regselOESource; //regsel < useq
	wire regselLoadSource; //regsel < useq
	wire [2:0] regselOEuSel; //regsel < useq
	wire [2:0] regselLoaduSel; //regsel < useq
	wire [2:0] regselOp0; //regsel < ir
	wire [2:0] regselOp1; //regsel < ir
	wire [2:0] regselOp2; //regsel < ir
	wire [7:0] regselRegOEs; //regsel > r0..r7
	wire [7:0] regselRegNotLoads; //regsel > r0..r7
	regSel _regsel(regselOE, regselLoad, regselOESource, regselLoadSource, regselOEuSel, regselLoaduSel, regselOp0, regselOp1, regselOp2, regselRegOEs, regselRegNotLoads);

	//general purpose registers
	wire pcInc;
	regFile registers(clock, aBus, yBus, regselRegOEs, regselRegNotLoads, notReset, pcInc);

	//status
	wire statusCIn; //status < alu
	wire statusCOut; //status >  alu, useq
	wire statusZIn; //status < alu
	wire statusZOut; //status > useq
	wire statusNotLoad; //status < useq
	statusRegister status(clock, statusNotLoad, statusCIn, statusCOut, statusZIn, statusZOut);

	//ALU
	wire aluNotALUOE; //alu < useq
	wire aluNotShiftOE; //alu < useq
	wire [4:0] aluF; //alu < useq
	wire aluCSel; //alu < useq
	wire aluUCIn; //alu < useq
	alu _alu(aBus, aluB, yBus, aluF, aluCSel, aluUCIn, statusCOut, statusCIn, statusZIn, aluNotALUOE, aluNotShiftOE);
	
	//B register
	wire[15:0] aluB; //alu < aluBReg
	wire aluBRegNotLoad; //aluBReg < useq
	bRegister breg(clock, aluBRegNotLoad, aBus, aluB);
	
	//instruction register
	wire irNotLoad; //ir < useq
	wire [6:0] irOpcode; //ir < useq
	instructionRegister ir(clock, irNotLoad, yBus, irOpcode, regselOp0, regselOp1, regselOp2);
		
	//micro sequencer
	wire [33:0] control;
	mSeq #(.ROM_FILENAME("urom.lst")) _mSeq(clock, notReset, irOpcode, statusCOut, statusZOut, control);
	
	//error signals
	wire [1:0] error;
	
	//read & write signals
	rwRegister _rwRegister(clock, notReset, memWrite, memRead, memNotWrite, memNotRead);
	
	wire memRead;
	wire memWrite;
		
	assign error = control[27:26]; //2, H
	assign statusNotLoad = control[25]; //1,L
	assign regselOE = control[24]; //1,H
	assign regselLoad = control[23]; //1,H
	assign regselOESource = control[22:21]; //2,H
	assign regselLoadSource = control[20]; //1,H
	assign regselOEuSel = control[19:17]; //3,H
	assign regselLoaduSel = control[16:14]; //3,H
	assign pcInc = control[13]; //1,H
	assign aluBRegNotLoad = control[12]; //1,L
	assign aluNotALUOE = control[11]; //1,L
	assign aluF = control[10:6]; //5,H
	assign aluNotShiftOE = control[5]; //1,L
	assign aluCSel = control[4]; //1,H
	assign aluUCIn = control[3]; //1,H
	assign memRead = control[2]; //1,L
	assign memWrite = control[1]; //1,L
	assign irNotLoad = control[0]; //1,L

endmodule
