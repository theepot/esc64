`ifndef _BREG_INCLUDED_
`define _BREG_INCLUDED_

`include "octRegister_74377.v"

module bRegister(clock, notLoad, in, out);
	input clock, notLoad;
	input [15:0] in;
	wire [15:0] in;
	
	output [15:0] out;
	wire [15:0] out;
	
	octRegister_74377 regH(clock, notLoad, in[15:8], out[15:8]);
	octRegister_74377 regL(clock, notLoad, in[7:0], out[7:0]);

endmodule

`endif
