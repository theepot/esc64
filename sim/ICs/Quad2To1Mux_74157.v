`ifndef _QUAD_2_TO_1_MUX_74157_INCLUDED_
`define _QUAD_2_TO_1_MUX_74157_INCLUDED_

module Quad2To1Mux_74157(notIE, sel, inA, inB, out);
	input notIE;
	input sel;
	output[3:0] out;
	input[3:0] inA;
	input[3:0] inB;

	assign #(25+15) out = !notIE ? (sel ? inB : inA) : 4'b0000;

endmodule

`endif
