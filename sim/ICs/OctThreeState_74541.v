`ifndef _OCT_THREE_STATE_74541_INCLUDED_
`define _OCT_THREE_STATE_74541_INCLUDED_

module OctThreeState_74541(notOE1, notOE2, in, out);
	input notOE1, notOE2;
	wire notOE1, notOE2;
	
	input[7:0] in;
	wire[7:0] in;
	
	output[7:0] out;
	wire[7:0] out;

	assign #(30+15) out = ~(notOE1 | notOE2) ? in : 8'bzzzzzzzz;
endmodule

`endif

