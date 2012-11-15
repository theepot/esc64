`include "../sram/sram.v"

`ifdef ALL_STRUCT
	`define GPREGISTER_STRUCT 1
	`define PC_STRUCT 1
	`define STATUS_STRUCT 1
	`define IR_STRUCT 1
	`define ALU_STRUCT 1
	`define REGSEL_STRUCT 1
	`define BREG_STRUCT 1
	`define MSEQ_STRUCT 1
`endif

`ifdef GPREGISTER_STRUCT
`include "../GPRegister/GPRegister_s.v"
`else
//`include "../GPRegister/GPRegister.v"
`endif

`ifdef PC_STRUCT
`include "../pc/pc_s.v"
`else
//`include "../pc/pc.v"
`endif

`ifdef STATUS_STRUCT
`include "../status/status_s.v"
`else
//`include "../status/status.v"
`endif

`ifdef IR_STRUCT
`include "../ir/ir_s.v"
`else
//`include "../ir/ir.v"
`endif

`ifdef ALU_STRUCT
`include "../alu/alu_s.v"
`else
//`include "../alu/alu.v"
`endif

`ifdef REGSEL_STRUCT
`include "../regsel/regsel_s.v"
`else
//`include "../regsel/regsel.v"
`endif

`ifdef BREG_STRUCT
`include "../74xxx/octRegister_74377.v"
`else
//`include "../register/register.v"
`endif

`ifdef MSEQ_STRUCT
`include "../mSeq/mSeq_s.v"
`else
//`include "../mSeq/mSeq.v"
`endif

module cpu();
	//misc
	wire [15:0] aBus;
	wire [15:0] yBus;
	reg clock;
	reg notReset;

	//register selection
	wire regselOE; //regsel < useq
	wire regselLoad; //regsel < useq
	wire [1:0] regselOESourceSel; //regsel < useq
	wire regselLoadSourceSel; //regsel < useq
	wire [2:0] regselUSeqRegSelOE; //regsel < useq
	wire [2:0] regselUSeqRegSelLoad; //regsel < useq
	wire [2:0] regselOp0; //regsel < ir
	wire [2:0] regselOp1; //regsel < ir
	wire [2:0] regselOp2; //regsel < ir
	wire [7:0] regselRegOEs; //regsel > r0..r7
	wire [7:0] regselRegNotLoads; //regsel > r0..r7

	regSel _regsel(regselOE, regselLoad, regselOESourceSel, regselLoadSourceSel, regselUSeqRegSelOE, regselUSeqRegSelLoad, regselOp0, regselOp1, regselOp2, regselRegOEs, regselRegNotLoads);

	//registers
	wire r0NotOE; //r0 < regsel
	wire r0NotLoad; //r0 < regsel
	assign r0NotOE = regselRegOEs[0];
	assign r0NotLoad = regselRegNotLoads[0];
	
	wire r1NotOE; //r1 < regsel
	wire r1NotLoad; //r1 < regsel
	assign r1NotOE = regselRegOEs[1];
	assign r1NotLoad = regselRegNotLoads[1];
	
	wire r2NotOE; //r2 < regsel
	wire r2NotLoad; //r2 < regsel
	assign r2NotOE = regselRegOEs[2];
	assign r2NotLoad = regselRegNotLoads[2];
	
	wire r3NotOE; //r3 < regsel
	wire r3NotLoad; //r3 < regsel
	assign r3NotOE = regselRegOEs[3];
	assign r3NotLoad = regselRegNotLoads[3];
	
	wire r4NotOE; //r4 < regsel
	wire r4NotLoad; //r4 < regsel
	assign r4NotOE = regselRegOEs[4];
	assign r4NotLoad = regselRegNotLoads[4];
	
	wire r5NotOE; //r5 < regsel
	wire r5NotLoad; //r5 < regsel
	assign r5NotOE = regselRegOEs[5];
	assign r5NotLoad = regselRegNotLoads[5];
	
	wire lrNotOE; //lr < regsel
	wire lrNotLoad; //lr < regsel
	assign lrNotOE = regselRegOEs[6];
	assign lrNotLoad = regselRegNotLoads[6];
	
	wire pcNotOE; //pc < regsel
	wire pcNotLoad; //pc < regsel
	assign pcNotOE = regselRegOEs[7];
	assign pcNotLoad = regselRegNotLoads[7];
	wire pcInc; //pc < useq
	
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
	wire aluNotALUOE; //alu < useq
	wire aluNotShiftOE; //alu < useq
	
`ifdef BREG_STRUCT
	octRegister_74377 bRegL(clock, aluBRegNotLoad, aBus[7:0], aluB[7:0]);
	octRegister_74377 bRegH(clock, aluBRegNotLoad, aBus[15:8], aluB[15:8]);
`else
	register aluBReg(clock, notReset, aluBRegNotLoad, 1, aBus, aluB);
`endif

	wire [4:0] aluF; //alu < useq
	wire aluCSel; //alu < useq
	wire aluUCIn; //alu < useq
	alu _alu(aBus, aluB, yBus, aluF, aluCSel, aluUCIn, statusCOut, statusCIn, statusZIn, aluNotALUOE, aluNotShiftOE);
	
	//ram
	wire memNotOE; //mem < useq
	wire memNotWE; //mem < useq
	wire memNotCS; //mem < useq
	sram #(.MEMFILE("ram.lst")) ram(aBus, yBus, memNotOE, memNotWE, memNotCS);
	
	//instruction register
	wire irNotLoad; //ir < useq
	wire [6:0] irOpcode; //ir < useq
	instructionRegister ir(clock, irNotLoad, yBus, irOpcode, regselOp0, regselOp1, regselOp2);
		
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
	assign aluNotALUOE = control[12]; //1,L
	assign aluF = control[11:7]; //5,H
	assign aluNotShiftOE = control[6]; //1,L
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

		
		#(800*2*1000)
		$display("ERROR: computer did not halt in 1000 cycles");
		$finish;

	end

	always begin
		#800 clock = ~clock;
		if(irOpcode == 7'b1111111) begin
			#5 $writememb("dump0.lst", ram.mem, 0, 64);
			$writememb("dump1.lst", ram.mem, (1<<16) - 100, (1<<16)-1);
			$finish;
		end
	end

endmodule
