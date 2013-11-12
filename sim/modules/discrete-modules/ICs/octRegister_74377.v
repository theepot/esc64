`ifndef _OCT_REGISTER_74377_INCLUDED_
`define _OCT_REGISTER_74377_INCLUDED_

module octRegister_74377(clock, notLoad, in, out);
	input clock, notLoad;
	input [7:0] in;
	output [7:0] out;
	reg [7:0] out;

	always @ (posedge clock)
	if (~notLoad) begin
		#(35+15) out = in;
	end

endmodule

`endif
