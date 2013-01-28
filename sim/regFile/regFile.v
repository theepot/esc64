`ifndef _REGFILE_INCLUDED_
`define _REGFILE_INCLUDED_

`ifdef GPREGISTER_STRUCT
`include "../GPRegister/GPRegister_s.v"
`else
`include "../GPRegister/GPRegister.v"
`endif

`ifdef PC_STRUCT
`include "../pc/pc_s.v"
`else
`include "../pc/pc.v"
`endif

module regFile(clock, aBus, yBus, notOE, notLoad, pcNotReset, pcInc);
	input clock, pcNotReset, pcInc;
	input [7:0] notOE, notLoad;
	inout [15:0] aBus;
	
	inout [15:0] yBus;
	
	GPRegister r[6:0] (clock, notLoad[6:0], notOE[6:0], yBus, aBus);
	program_counter pc(clock, pcNotReset, notLoad[7], notOE[7], pcInc, yBus, aBus);
endmodule

`endif
