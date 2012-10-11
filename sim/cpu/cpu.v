`include "../alu/alu.v"
`include "../pc/pc.v"
`include "../register/register.v"
`include "../regsel/regsel.v"
`include "../mSeq/mSeq.v"

module cpu();
	parameter DATA_BUS_WIDTH = 16;

	//misc
	wire [DATA_BUS_WIDTH-1:0] dataBus;
	wire clock;
	wire reset;

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
	wire [7:0] regselRegLoads; //regsel <> r0..r7

	regSel _regsel(regselOE, regselLoad, regselOESourceSel, regselLoadSourceSel, regselUSeqRegSelOE, regselUSeqRegSelLoad, regselOp0, regselOp1, regselOp2, regselRegOEs, regselRegLoads);


	//registers
	wire r0OE; //r0 <> regsel
	wire r0Load; //r0 <> regsel
	assign r0OE = regselRegOEs[0];
	assign r0Load = regselRegLoads[0];
	
	wire r1OE; //r1 <> regsel
	wire r1Load; //r1 <> regsel
	assign r1OE = regselRegOEs[1];
	assign r1Load = regselRegLoads[1];
		
	wire r2OE; //r2 <> regsel
	wire r2Load; //r2 <> regsel
	assign r2OE = regselRegOEs[2];
	assign r2Load = regselRegLoads[2];
	
	wire r3OE; //r3 <> regsel
	wire r3Load; //r3 <> regsel
	assign r3OE = regselRegOEs[3];
	assign r3Load = regselRegLoads[3];
	
	wire r4OE; //r4 <> regsel
	wire r4Load; //r4 <> regsel
	assign r4OE = regselRegOEs[4];
	assign r4Load = regselRegLoads[4];
	
	wire lrOE; //lr <> regsel
	wire lrLoad; //lr <> regsel
	assign lrOE = regselRegOEs[5];
	assign lrLoad = regselRegLoads[5];
	
	wire spOE; //sp <> regsel
	wire spLoad; //sp <> regsel
	assign spOE = regselRegOEs[6];
	assign spLoad = regselRegLoads[6];
	
	wire pcOE; //pc <> regsel
	wire pcLoad; //pc <> regsel
	assign pcOE = regselRegOEs[7];
	assign pcLoad = regselRegLoads[7];
	wire pcInc; //pc <> useq
	
	register r0(clock, reset, r0OE, r0Load, dataBus, dataBus);
	register r1(clock, reset, r1OE, r1Load, dataBus, dataBus);
	register r2(clock, reset, r2OE, r2Load, dataBus, dataBus);
	register r3(clock, reset, r3OE, r3Load, dataBus, dataBus);
	register r4(clock, reset, r4OE, r4Load, dataBus, dataBus);
	register lr(clock, reset, r5OE, r5Load, dataBus, dataBus);
	register sp(clock, reset, r6OE, r6Load, dataBus, dataBus);
	program_counter pc(clock, reset/*TODO <- is active low*/, pcLoad/*TODO <- is active low*/, pcOE, pcInc, dataBus, dataBus);

	//ALU
	wire[15:0] aluB; //alu <> aluBReg
	wire aluBRegOE; //aluBReg <> useq
	wire aluBRegLoad; //aluBReg <> useq
	register aluBReg(clock, reset, aluBRgegOE, aluBRegLoad, dataBus, aluB);

	wire[15:0] aluY; //aluYReg <> alu
	wire aluYRegOE; //aluYRegOE <> useq
	wire aluYRegLoad; //aluYRegLoad <> useq
	register aluYReg(clock, reset, aluYRegOE, aluYRegLoad, aluY, dataBus);
	
	wire [4:0] aluF; //alu <> useq
	wire aluFSel; //alu <> useq
	wire aluCSel; //alu <> useq
	wire aluUCIn; //alu <> useq
	wire aluFCin; //alu <> useq
	wire aluCout; //alu <> TODO:statusC
	wire aluZout; //alu <> TODO:statusZ
	alu _alu(dataBus, aluB, aluY, aluF, aluFSel, aluCSel, aluUCIn, aluFCin, aluCout, aluZout);
	
	//micro sequencer
	wire [15:0] control;
	mSeq _mSeq(clock, reset, /*TODO: opcode, carry, zero,*/ control);
	
	assign control[0] = regselOE;
	assign control[1] = regselLoad;
	assign control[2] = regselOESourceSel;
	assign control[3] = regselLoadSourceSel;
	assign control[4] = regselUSeqRegSelOE;
	assign control[5] = regselUSeqRegSelLoad;
	assign control[6] = pcInc;
	assign control[7] = aluBRegOE;
	assign control[8] = aluBRegLoad;
	assign control[9] = aluYRegOE;
	assign control[10] = aluYRegLoad;
	assign control[11] = aluF;
	assign control[12] = aluFSel;
	assign control[13] = aluCSel;
	assign control[14] = aluUCIn;
	assign control[15] = aluFCin;

endmodule
