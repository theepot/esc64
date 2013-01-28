`ifndef _QUADCOUNTER_74163_INCLUDED_
`define _QUADCOUNTER_74163_INCLUDED_

module QuadCounter_74163(clock, notClear, notLoad, enp, ent, in, out, cout);
	input clock, notClear, notLoad, enp, ent;
	input [3:0] in;
	output [3:0] out;
	output cout;

	reg [3:0] out;

	always @ (posedge clock)
		begin
			
			if (~notClear) begin
				#(43+15) out = 0;
			end else if (~notLoad) begin
				#(43+15) out = in;
			end else if (enp & ent) begin
				#(43+15) out = out + 1;
			end
		end
		
	assign cout = (out == 15);


endmodule

`endif
