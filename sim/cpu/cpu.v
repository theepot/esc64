`include "../alu/alu.v"
`include "../pc/pc.v"
`include "../register/register.v"
`include "../regsel/regsel.v"
`include "../mSeq/mSeq.v"
`include "../sram/sram.v"
`include "../ir/ir.v"
`include "../status/status.v"

module cpu();
	//misc
	wire [15:0] aBus;
	wire [15:0] yBus;
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
	
	register r0(clock, notReset, r0NotLoad, r0OE, yBus, aBus);
	register r1(clock, notReset, r1NotLoad, r1OE, yBus, aBus);
	register r2(clock, notReset, r2NotLoad, r2OE, yBus, aBus);
	register r3(clock, notReset, r3NotLoad, r3OE, yBus, aBus);
	register r4(clock, notReset, r4NotLoad, r4OE, yBus, aBus);
	register lr(clock, notReset, lrNotLoad, lrOE, yBus, aBus);
	register sp(clock, notReset, spNotLoad, spOE, yBus, aBus);
	program_counter pc(clock, notReset, pcNotLoad, pcOE, pcInc, yBus, aBus);

	//status
	wire statusCIn; //status < alu
	wire statusCOut; //status >  alu, useq
	wire statusZIn; //status < alu
	wire statusZOut; //status > alu, useq
	wire statusNotLoad; //status < useq
	statusRegister status(clock, statusNotLoad, statusCIn, statusCOut, statusZIn, statusZOut);

	//ALU
	wire[15:0] aluB; //alu < aluBReg
	wire aluBRegNotLoad; //aluBReg < useq
	wire aluYOE; //alu < useq
	register aluBReg(clock, notReset, aluBRegNotLoad, 1, aBus, aluB);
	
	wire [4:0] aluF; //alu <> useq
	wire aluFSel; //alu <> useq
	wire aluCSel; //alu <> useq
	wire aluUCIn; //alu <> useq
	alu _alu(aBus, aluB, yBus, aluF, aluFSel, aluCSel, aluUCIn, statusCOut, statusCIn, statusZIn, aluYOE);
	
	//ram
	wire memNotOE; //mem <> useq
	wire memNotWE; //mem <> useq
	wire memNotCS; //mem <> useq
	sram #(.MEMFILE("ram.lst")) ram(aBus, yBus, memNotOE, memNotWE, memNotCS);
	
	//instruction register
	wire irNotLoad; //ir <> useq
	wire [6:0] irOpcode; //ir <> useq
	
	instructionRegister ir(clock, notReset, irNotLoad, yBus, irOpcode, regselOp0, regselOp1, regselOp2);
		
	//micro sequencer
	wire [26:0] control;
	mSeq #(.ROM_FILENAME("urom.lst")) _mSeq(clock, notReset, irOpcode, statusCOut, statusZOut, control);
	
	assign statusNotLoad = control[26]; //1,L
	assign regselOE = control[25]; //1,H
	assign regselLoad = control[24]; //1,H
	assign regselOESourceSel = control[23:22]; //2,H
	assign regselLoadSourceSel = control[21]; //1,H
	assign regselUSeqRegSelOE = control[20:18]; //3,H
	assign regselUSeqRegSelLoad = control[17:15]; //3,H
	assign pcInc = control[14]; //1,H
	assign aluBRegNotLoad = control[13]; //1,L
	assign aluYOE = control[12]; //1,H
	assign aluF = control[11:7]; //5,H
	assign aluFSel = control[6]; //1,H
	assign aluCSel = control[5]; //1,H
	assign aluUCIn = control[4]; //1,H
	assign memNotOE = control[3]; //1,L
	assign memNotWE = control[2]; //1,L
	assign memNotCS = control[1]; //1,L
	assign irNotLoad = control[0]; //1,L
	
	initial begin
		$dumpfile("cpu.vcd");
		$dumpvars(0);
		
		notReset = 0;
		clock = 0;
		#7 notReset = 1;
		
		#99999 $finish;
	end

	always begin
		#5 clock = ~clock;
		if(ir.ir.data == 16'b1111111_000_000_000) begin
			#5 $writememb("dump.lst", ram.mem, 0, 64);
			$finish;
		end
	end

endmodule
