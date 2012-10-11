`include "../alu/alu.v"
`include "../pc/pc.v"
`include "../register/register.v"
`include "../regsel/regsel.v"

module cpu();
	parameter DATA_BUS_WIDTH = 16;

	//misc
	wire dataBus[DATA_BUS_WIDTH-1:0];
	wire clock;
	wire reset;

	//register selection
	wire regselOE;
	wire regselLoad;
	wire regselOESourceSel;
	wire regselLoadSourceSel;
	wire regselUSeqRegSelOE;
	wire regselUSeqRegSelLoad;
	wire regselOp0;
	wire regselOp1;
	wire regselOp2;
	wire [4:0] regselRegOEs;
	wire [4:0] regselRegLoads;

	regsel _regsel(regselOE, regselLoad, regselOESourceSel, regselLoadSourceSel, regselUSeqRegSelOE, regselUSeqRegSelLoad, regselOp0, regselOp1, regselOp2, regselRegOEs, regselRegLoads);


	//registers
	wire pcInc;
	
	register r0(clock, reset, /*output_enable, load*/, dataBus, dataBus);
	register r1(clock, reset, /*output_enable, load*/, dataBus, dataBus);
	register r2(clock, reset, /*output_enable, load*/, dataBus, dataBus);
	register r3(clock, reset, /*output_enable, load*/, dataBus, dataBus);
	register r4(clock, reset, /*output_enable, load*/, dataBus, dataBus);
	register lr(clock, reset, /*output_enable, load*/, dataBus, dataBus);
	register sp(clock, reset, /*output_enable, load*/, dataBus, dataBus);
	program_counter pc(clock, /*notClr*/, /*notWrite*/, /*read*/, pcInc, dataBus, dataBus);

	//ALU
	wire aluB;
	wire aluY;
	
	register aluBReg(clock, reset, /*output_enable, load*/, dataBus, aluB);
	register aluYReg(clock, reset, /*output_enable, load*/, aluY, dataBus);
	
	alu _alu(dataBus, aluB, aluY, /*f, fsel, csel, ucin, fcin, cout, zout*/);
	
endmodule
