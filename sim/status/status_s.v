`ifndef _STATUS_S_INCLUDED_
`define _STATUS_S_INCLUDED_

`include "../74xxx/octRegister_74377.v"

module statusRegister(clock, notLoad, cIn, cOut, zIn, zOut);
	input clock, cIn, zIn, notLoad;
	output cOut, zOut;
	
	wire[7:0] rIn;
	assign rIn = 8'bxxxxxx00 | (cIn << 1) | zIn;
	
	wire[7:0] rOut;
	assign cOut = rOut & (1 << 1) ? 1 : 0;
	assign zOut = rOut & 1 ? 1 : 0;
	
	initial begin
		r.out = 8'bxxxxxx00;
	end
	
	octRegister_74377 r(clock, notLoad, rIn, rOut);
	
endmodule

`endif
