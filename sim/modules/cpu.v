`include "status.v"
`include "ir.v"
`include "alu.v"
`include "regsel.v"
`include "breg.v"
`include "mSeq.v"
`include "regFile.v"
`include "io_interface.v"

module cpu(clock, notReset, address, data, memNotRead, memNotWrite, csh_n, csl_n, select_dev, cpu_inspect, error, at_fetch);
	input clock, notReset, cpu_inspect;
	output memNotRead, memNotWrite, csh_n, csl_n, select_dev, at_fetch;
	output [1:0] error;
	inout [15:0] data;
	inout [14:0] address;
		
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
	wire [41:0] control;
	mSeq #(.ROM_FILENAME("urom.lst")) _mSeq(clock, notReset, irOpcode, statusCOut, statusZOut, cpu_inspect, control);
	
	//io interface
	wire memRead, memWrite, io_select_dev, io_idle_n, io_word, io_dir_out, io_address_reg_ld_n, io_data_reg_ld_n;
	io_interface _io_interface(clock, notReset, aBus, yBus, memRead, memWrite, io_select_dev, io_idle_n, io_word, io_dir_out, io_address_reg_ld_n, io_data_reg_ld_n, address, data, memNotRead, memNotWrite, csl_n, csh_n, select_dev);
	
	assign at_fetch = control[34];//1, H
	assign io_data_reg_ld_n = control[33];//1, L
	assign io_address_reg_ld_n = control[32];//1, L
	assign io_select_dev = control[31];//1, H
	assign io_dir_out = control[30];//1, H
	assign io_word = control[29];//1, H
	assign io_idle_n = control[28];//1, L
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
