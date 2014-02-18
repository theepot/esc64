`ifndef _OCTBUSTRANSCEIVER_74245_
`define _OCTBUSTRANSCEIVER_74245_

module OctBusTransceiver_74245(oe_n, dir, a, b);
	input oe_n, dir;
	inout [7:0] a,b;
	
	wire oe_n, dir;
	wire [7:0] a,b;
	
	assign #(30 + 12) a = ~oe_n && ~dir ? b : 8'bZ;
	assign #(30 + 12) b = ~oe_n &&  dir ? a : 8'bZ;
	
endmodule


`endif

