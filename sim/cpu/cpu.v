`include "../alu/alu.v"
`include "../pc/pc.v"
`include "../register/register.v"
`include "../regsel/regsel.v"
`include "../mSeq/mSeq.v"
`include "../sram/sram.v"
`include "../ir/ir.v"

module cpu();
	//misc
	wire [15:0] dataBus;
	wire clock;
	wire notReset;

	//register selection
	wire regselOE; //regsel <> useq
	wire regselLoad; //regsel <> useq
	wire [1:0] regselOESourceSel; //regsel <> useq
	wire regselLoadSourceSel; //regsel <> useq
	wire [2:0] regselUSeqRegSelOE; //regsel <> useq
	wire [2:0] regselUSeqRegSelLoad; //regsel <> useq
	wire [2:0] regselOp0; //regsel <> ir
	wire [2:0] regselOp1; //regsel <> ir
	wire [2:0] regselOp2; //regsel <> ir
	wire [7:0] regselRegOEs; //regsel <> r0..r7
	wire [7:0] regselRegNotLoads; //regsel <> r0..r7

	regSel _regsel(regselOE, regselLoad, regselOESourceSel, regselLoadSourceSel, regselUSeqRegSelOE, regselUSeqRegSelLoad, regselOp0, regselOp1, regselOp2, regselRegOEs, regselRegNotLoads);

	//registers
	wire r0OE; //r0 <> regsel
	wire r0Load; //r0 <> regsel
	assign r0OE = regselRegOEs[0];
	assign r0Load = regselRegNotLoads[0];
	
	wire r1OE; //r1 <> regsel
	wire r1NotLoad; //r1 <> regsel
	assign r1OE = regselRegOEs[1];
	assign r1NotLoad = regselRegNotLoads[1];
		
	wire r2OE; //r2 <> regsel
	wire r2NotLoad; //r2 <> regsel
	assign r2OE = regselRegOEs[2];
	assign r2NotLoad = regselRegNotLoads[2];
	
	wire r3OE; //r3 <> regsel
	wire r3NotLoad; //r3 <> regsel
	assign r3OE = regselRegOEs[3];
	assign r3NotLoad = regselRegNotLoads[3];
	
	wire r4OE; //r4 <> regsel
	wire r4NotLoad; //r4 <> regsel
	assign r4OE = regselRegOEs[4];
	assign r4NotLoad = regselRegNotLoads[4];
	
	wire lrOE; //lr <> regsel
	wire lrNotLoad; //lr <> regsel
	assign lrOE = regselRegOEs[5];
	assign lrNotLoad = regselRegNotLoads[5];
	
	wire spOE; //sp <> regsel
	wire spNotLoad; //sp <> regsel
	assign spOE = regselRegOEs[6];
	assign spNotLoad = regselRegNotLoads[6];
	
	wire pcOE; //pc <> regsel
	wire pcNotLoad; //pc <> regsel
	assign pcOE = regselRegOEs[7];
	assign pcNotLoad = regselRegNotLoads[7];
	wire pcInc; //pc <> useq
	
	register r0(clock, notReset, r0NotLoad, r0OE, dataBus, dataBus);
	register r1(clock, notReset, r1NotLoad, r1OE, dataBus, dataBus);
	register r2(clock, notReset, r2NotLoad, r2OE, dataBus, dataBus);
	register r3(clock, notReset, r3NotLoad, r3OE, dataBus, dataBus);
	register r4(clock, notReset, r4NotLoad, r4OE, dataBus, dataBus);
	register lr(clock, notReset, r5NotLoad, r5OE, dataBus, dataBus);
	register sp(clock, notReset, r6NotLoad, r6OE, dataBus, dataBus);
	program_counter pc(clock, notReset, pcNotLoad, pcOE, pcInc, dataBus, dataBus);

	//status
	wire statusOE; //status <> useq
	wire statusNotLoad; //status <> useq
	wire [15:0] statusIn; //status <> statusCIn, statusZIn
	wire [15:0] statusOut; //status <> statusCOut, statusZOut;

	wire statusCIn; //status <> alu
	wire statusCOut; //status <>  alu, useq
	assign statusIn[0] = statusCIn;
	assign statusOut[0] = statusCOut;
	
	wire statusZIn; //status <> alu
	wire statusZOut; //status <> alu, useq
	assign statusIn[1] = statusZIn;
	assign statusOut[1] = statusZOut;
	
	register status(clock, notReset, statusNotLoad, statusOE, statusIn, statusOut);

	//ALU
	wire[15:0] aluB; //alu <> aluBReg
	wire aluBRegOE; //aluBReg <> useq
	wire aluBRegNotLoad; //aluBReg <> useq
	register aluBReg(clock, notReset, aluBRegNotLoad, aluBRgegOE, dataBus, aluB);

	wire[15:0] aluY; //aluYReg <> alu
	wire aluYRegOE; //aluYRegOE <> useq
	wire aluYRegNotLoad; //aluYRegLoad <> useq
	register aluYReg(clock, notReset, aluYRegNotLoad ,aluYRegOE, aluY, dataBus);
	
	wire [4:0] aluF; //alu <> useq
	wire aluFSel; //alu <> useq
	wire aluCSel; //alu <> useq
	wire aluUCIn; //alu <> useq
	wire aluFCin; //alu <> useq
	alu _alu(dataBus, aluB, aluY, aluF, aluFSel, aluCSel, aluUCIn, aluFCin, statusCOut, aluZOut);
		
	//ram
	wire addrRegOE; //addrReg <> useq
	wire addrRegLoad; //addrReg <> useq
	wire [15:0] addrRegOut; //addrReg <> ram
	register addrReg(clock, notReset, addrRegNotLoad, addrRegOE, dataBus, addrRegOut);
	
	wire memNotOE; //mem <> useq
	wire memNotWE; //mem <> useq
	wire memNotCS; //mem <> useq
	sram ram(addrRegOut, dataBus, memNotOE, memNotWE, memNotCS);
		
	//instruction register
	wire irOE; //ir <> useq
	wire irNotLoad; //ir <> useq
	wire [6:0] irOpcode; //ir <> useq
	
	instructionRegister ir(clock, notReset, irNotLoad, irOE, dataBus, irOpcode, regselOp0, regselOp1, regselOp2);
		
	//micro sequencer
	wire [33:0] control;
	mSeq _mSeq(clock, reset, irOpcode, statusCOut, statusZOut, control);
	
	assign control[0] = regselOE;
	assign control[1] = regselLoad;
	assign control[2] = regselOESourceSel; //2
	assign control[4] = regselLoadSourceSel;
	assign control[5] = regselUSeqRegSelOE; //3
	assign control[8] = regselUSeqRegSelLoad; //3
	assign control[11] = pcInc;
	assign control[12] = statusOE;
	assign control[13] = statusNotLoad;
	assign control[14] = aluBRegOE;
	assign control[15] = aluBRegNotLoad;
	assign control[16] = aluYRegOE;
	assign control[17] = aluYRegNotLoad;
	assign control[18] = aluF; //5
	assign control[23] = aluFSel;
	assign control[24] = aluCSel;
	assign control[25] = aluUCIn;
	assign control[26] = aluFCin;
	assign control[27] = addrRegOE;
	assign control[28] = addrRegNotLoad;
	assign control[29] = memNotOE;
	assign control[30] = memNotWE;
	assign control[31] = memNotCS;
	assign control[32] = irOE;
	assign control[33] = irNotLoad;

	initial begin
		
		
		$finish;
	end

endmodule































