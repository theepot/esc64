`ifndef OCT_INPUT_OR_NOR_744078
`define OCT_INPUT_OR_NOR_744078

module OctInputOrNor_744078(in, out, notOut);
	input[7:0] in;
	output out, notOut;
	
	assign #33 out		= in != 8'b00000000 ? 1'b1 : 1'b0;
	assign #33 notOut	= in != 8'b00000000 ? 1'b0 : 1'b1;
endmodule

`endif
