`ifndef OCT_THREE_STATE_74541_INCLUDED
`define OCT_THREE_STATE_74541_INCLUDED

module OctThreeState_74541(notOE1, notOE2, in, out);
	input notOE1, notOE2;
	wire notOE1, notOE2;
	
	input[7:0] in;
	wire[7:0] in;
	
	output[7:0] out;
	wire[7:0] out;

	assign #38 out = ~(notOE1 | notOE2) ? in : 8'bzzzzzzzz;
endmodule

`endif

