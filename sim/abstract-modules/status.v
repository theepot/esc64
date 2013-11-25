`ifndef _STATUS_INCLUDED_
`define _STATUS_INCLUDED_

`include "register.v"

module statusRegister(clock, notLoad, cIn, cOut, zIn, zOut);
	input clock, cIn, zIn, notLoad;
	output cOut, zOut;
	
	initial begin
	status.data = 0;
	end
	
	register #(.DATA_WIDTH(2)) status(clock, 1, notLoad, 1, {cIn, zIn}, {cOut, zOut});
	
endmodule

`endif
