`include "../alu/alu.v"
`include "../pc/pc.v"
`include "../register/register.v"
`include "../regsel/regsel.v"
`include "../mSeq/mSeq.v"
`include "../sram/sram.v"
`include "../ir/ir.v"
`include "../status/status.v"

`ifdef GPREGISTER_STRUCT
`include "../GPRegister/GPRegister_s.v"
`else
`include "../GPRegister/GPRegister.v"
`endif

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
	wire r0NotOE; //r0 <> regsel
	wire r0NotLoad; //r0 <> regsel
	assign r0NotOE = regselRegOEs[0];
	assign r0NotLoad = regselRegNotLoads[0];
	
	wire r1NotOE; //r1 <> regsel
	wire r1NotLoad; //r1 <> regsel
	assign r1NotOE = regselRegOEs[1];
	assign r1NotLoad = regselRegNotLoads[1];
		
	wire r2NotOE; //r2 <> regsel
	wire r2NotLoad; //r2 <> regsel
	assign r2NotOE = regselRegOEs[2];
	assign r2NotLoad = regselRegNotLoads[2];
	
	wire r3NotOE; //r3 <> regsel
	wire r3NotLoad; //r3 <> regsel
	assign r3NotOE = regselRegOEs[3];
	assign r3NotLoad = regselRegNotLoads[3];
	
	wire r4NotOE; //r4 <> regsel
	wire r4NotLoad; //r4 <> regsel
	assign r4NotOE = regselRegOEs[4];
	assign r4NotLoad = regselRegNotLoads[4];
	
	wire r5NotOE; //r5 <> regsel
	wire r5NotLoad; //r5 <> regsel
	assign r5NotOE = regselRegOEs[5];
	assign r5NotLoad = regselRegNotLoads[5];
	
	wire lrNotOE; //lr <> regsel
	wire lrNotLoad; //lr <> regsel
	assign lrNotOE = regselRegOEs[6];
	assign lrNotLoad = regselRegNotLoads[6];
	
	wire pcNotOE; //pc <> regsel
	wire pcNotLoad; //pc <> regsel
	assign pcNotOE = regselRegOEs[7];
	assign pcNotLoad = regselRegNotLoads[7];
	wire pcInc; //pc <> useq
	
	GPRegister r0(clock, r0NotLoad, r0NotOE, yBus, aBus);
	GPRegister r1(clock, r1NotLoad, r1NotOE, yBus, aBus);
	GPRegister r2(clock, r2NotLoad, r2NotOE, yBus, aBus);
	GPRegister r3(clock, r3NotLoad, r3NotOE, yBus, aBus);
	GPRegister r4(clock, r4NotLoad, r4NotOE, yBus, aBus);
	GPRegister r5(clock, r5NotLoad, r5NotOE, yBus, aBus);
	GPRegister lr(clock, lrNotLoad, lrNotOE, yBus, aBus);
	program_counter pc(clock, notReset, pcNotLoad, pcNotOE, pcInc, yBus, aBus);

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
		#900 notReset = 1;
		
		#999999 $finish;
	end

	always begin
		#800 clock = ~clock;
		if(ir.ir.data == 16'b1111111_000_000_000) begin
			#5 $writememb("dump0.lst", ram.mem, 0, 64);
			$writememb("dump1.lst", ram.mem, (1<<16) - 100, (1<<16)-1);
			$finish;
		end
	end

endmodule
