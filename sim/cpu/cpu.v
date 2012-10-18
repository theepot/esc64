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
	reg clock;
	reg notReset;

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
	wire r0NotLoad; //r0 <> regsel
	assign r0OE = regselRegOEs[0];
	assign r0NotLoad = regselRegNotLoads[0];
	
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
	register lr(clock, notReset, lrNotLoad, lrOE, dataBus, dataBus);
	register sp(clock, notReset, spNotLoad, spOE, dataBus, dataBus);
	program_counter pc(clock, notReset, pcNotLoad, pcOE, pcInc, dataBus, dataBus);

	//status
	wire statusOE; //status <> useq
	wire statusNotLoad; //status <> useq
	wire [1:0] statusIn; //status <> statusCIn, statusZIn
	wire [1:0] statusOut; //status <> statusCOut, statusZOut;

	wire statusCIn; //status <> alu
	wire statusCOut; //status <>  alu, useq
	assign statusIn[0] = statusCIn;
	assign statusCOut = statusOut[0];
	
	wire statusZIn; //status <> alu
	wire statusZOut; //status <> alu, useq
	assign statusIn[1] = statusZIn;
	assign statusZOut = statusOut[1];
	
	register #(.DATA_WIDTH(2)) status(clock, notReset, 0, 1, statusIn, statusOut);

	//ALU
	wire[15:0] aluB; //alu <> aluBReg
	wire aluBRegNotLoad; //aluBReg <> useq
	register aluBReg(clock, notReset, aluBRegNotLoad, 1, dataBus, aluB);

	wire[15:0] aluY; //aluYReg <> alu
	wire aluYRegOE; //aluYRegOE <> useq
	wire aluYRegNotLoad; //aluYRegLoad <> useq
	register aluYReg(clock, notReset, aluYRegNotLoad ,aluYRegOE, aluY, dataBus);
	
	wire [4:0] aluF; //alu <> useq
	wire aluFSel; //alu <> useq
	wire aluCSel; //alu <> useq
	wire aluUCIn; //alu <> useq
	alu _alu(dataBus, aluB, aluY, aluF, aluFSel, aluCSel, aluUCIn, statusCOut, statusCIn, statusZOut);
//	alu(a, b, y, f, fsel, csel, ucin, fcin, cout, zout);
		
	//ram
	wire addrRegNotLoad; //addrReg <> useq
	wire [15:0] addrRegOut; //addrReg <> ram
	register addrReg(clock, notReset, addrRegNotLoad, 1, dataBus, addrRegOut);
	
	wire memNotOE; //mem <> useq
	wire memNotWE; //mem <> useq
	wire memNotCS; //mem <> useq
	sram #(.MEMFILE("ram.lst")) ram(addrRegOut, dataBus, memNotOE, memNotWE, memNotCS);
	
	//instruction register
	wire irNotLoad; //ir <> useq
	wire [6:0] irOpcode; //ir <> useq
	
	instructionRegister ir(clock, notReset, irNotLoad, dataBus, irOpcode, regselOp0, regselOp1, regselOp2);
		
	//micro sequencer
	wire [33:0] control;
//	mSeq _mSeq(clock, reset, irOpcode, statusCOut, statusZOut, control);
	mSeq #(.ROM_FILENAME("rom.lst")) _mSeq(clock, notReset, irOpcode, statusCOut, statusZOut, control);
	
	/*assign control[33] = regselOE; //1,H
	assign control[32] = regselLoad; //1,H
	assign control[31:30] = regselOESourceSel; //2,H
	assign control[29] = regselLoadSourceSel; //1,H
	assign control[28:26] = regselUSeqRegSelOE; //3,H
	assign control[25:23] = regselUSeqRegSelLoad; //3,H
	assign control[22] = pcInc; //1,H
	assign control[21] = statusOE; //1,H
	assign control[20] = statusNotLoad; //1,L
	assign control[19] = aluBRegOE; //1,H
	assign control[18] = aluBRegNotLoad; //1,L
	assign control[17] = aluYRegOE; //1,H
	assign control[16] = aluYRegNotLoad; //1,L
	assign control[15:11] = aluF; //5,H
	assign control[10] = aluFSel; //1,H
	assign control[9] = aluCSel; //1,H
	assign control[8] = aluUCIn; //1,H
	assign control[7] = aluFCin; //1,H
	assign control[6] = addrRegOE; //1,H
	assign control[5] = addrRegNotLoad; //1,L
	assign control[4] = memNotOE; //1,L
	assign control[3] = memNotWE; //1,L
	assign control[2] = memNotCS; //1,L
	assign control[1] = irOE; //1,H
	assign control[0] = irNotLoad; //1,L*/
	
	assign regselOE = control[33]; //1,H
	assign regselLoad = control[32]; //1,H
	assign regselOESourceSel = control[31:30]; //2,H
	assign regselLoadSourceSel = control[29]; //1,H
	assign regselUSeqRegSelOE = control[28:26]; //3,H
	assign regselUSeqRegSelLoad = control[25:23]; //3,H
	assign pcInc = control[22]; //1,H
	assign statusOE = control[21]; //1,H
	assign statusNotLoad = control[20]; //1,L
	//assign aluBRegOE = control[19]; //1,H
	assign aluBRegNotLoad = control[18]; //1,L
	assign aluYRegOE = control[17]; //1,H
	assign aluYRegNotLoad = control[16]; //1,L
	assign aluF = control[15:11]; //5,H
	assign aluFSel = control[10]; //1,H
	assign aluCSel = control[9]; //1,H
	assign aluUCIn = control[8]; //1,H
	//assign aluFCin = control[7]; //1,H
	//assign addrRegOE = control[6]; //1,H
	assign addrRegNotLoad = control[5]; //1,L
	assign memNotOE = control[4]; //1,L
	assign memNotWE = control[3]; //1,L
	assign memNotCS = control[2]; //1,L
	assign irNotLoad = control[0]; //1,L
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0);
		//$dumpvars(0, dataBus, clock, notReset, regselOE, regselLoad, regselOESourceSel, regselLoadSourceSel, regselUSeqRegSelOE, regselUSeqRegSelLoad, regselOp0, regselOp1, regselOp2, regselRegOEs, regselRegNotLoads, r0OE, r0NotLoad, r1OE, r1NotLoad, r2OE, r2NotLoad, r3OE, r3NotLoad, r4OE, r4NotLoad, lrOE, lrNotLoad, spOE, spNotLoad, pcOE, pcNotLoad, pcInc, statusOE, statusNotLoad, statusIn, statusOut, statusCIn, statusCOut, statusZIn, statusZOut, aluB, aluBRegOE, aluBRegNotLoad, aluY, aluYRegOE, aluYRegNotLoad, aluF, aluFSel, aluCSel, aluUCIn, aluFCin, addrRegOE, addrRegNotLoad, addrRegOut, memNotOE, memNotWE, memNotCS, irOE, irNotLoad, irOpcode, control);
		//$dumpvars(0, clock, notcontrol
		
		notReset = 0;
		clock = 0;
		#7 notReset = 1;
		
		#500 $finish;
	end

	always begin
		#5 clock = ~clock;
	end

endmodule































