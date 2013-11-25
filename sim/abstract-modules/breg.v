`ifndef _BREG_INCLUDED_
`define _BREG_INCLUDED_

`include "register.v"

module bRegister(clock, notLoad, in, out);
	input clock, notLoad;
	input [15:0] in;
	wire [15:0] in;
	
	output [15:0] out;
	wire [15:0] out;
	
	register ir(clock, 1, notLoad, 1, in, out);
	
endmodule

`endif
