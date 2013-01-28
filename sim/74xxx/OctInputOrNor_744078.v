`ifndef _OCT_INPUT_OR_NOR_744078_
`define _OCT_INPUT_OR_NOR_744078_

module OctInputOrNor_744078(in, out, notOut);
	input[7:0] in;
	output out, notOut;
	
	assign #(26+15) out		= in != 8'b00000000 ? 1'b1 : 1'b0;
	assign #(26+15) notOut	= in != 8'b00000000 ? 1'b0 : 1'b1;
endmodule

`endif

