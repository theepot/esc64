`ifndef OCT_REGISTER_74273_INCLUDED
`define OCT_REGISTER_74273_INCLUDED

module octRegister_74273(clock, notReset, in, out);
	input clock, notReset;
	input [7:0] in;
	output [7:0] out;
	reg [7:0] out_int;
	
	assign #(15+30) out = notReset ? out_int : 8'H00;

	always @ (posedge clock) begin
		if(notReset) begin
			out_int = in;
		end
	end
	
	always @ (negedge notReset) begin
		out_int = 0;
	end
		
	

endmodule

`endif
