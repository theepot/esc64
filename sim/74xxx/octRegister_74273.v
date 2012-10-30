`ifndef OCT_REGISTER_74273_INCLUDED
`define OCT_REGISTER_74273_INCLUDED

module octRegister_74273(clock, notReset, in, out);
	input clock, notLoad;
	input [7:0] in;
	output [7:0] out;
	reg [7:0] outInt;

	always @ (posedge clock) begin
		#(15+30)
		if(notReset) begin
			out = in;
		end
	end
	
	always @ (negedge notReset) begin
		#(15+30) out = 0;
	end
	
	

endmodule

`endif
