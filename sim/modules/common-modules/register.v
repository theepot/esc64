`ifndef _REGISTER_INCLUDED_
`define _REGISTER_INCLUDED_

module register(clock, notReset, notLoad, OE, in, out);
	parameter DATA_WIDTH = 16;
	input clock, notReset, OE, notLoad;
	input [DATA_WIDTH-1:0] in;
	output [DATA_WIDTH-1:0] out;
	
	wire clock, notReset, OE, notLoad;
	wire [DATA_WIDTH-1:0] in;
	wire [DATA_WIDTH-1:0] out;
	
	reg [DATA_WIDTH-1:0] data;
	
	assign out = OE ? data : 32'bzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz;
	
	always @ (posedge clock)
	begin
		if(!notReset) begin
			data = 0;
		end	else begin
			if(!notLoad) begin
				data = in;
			end
		end
	
	end
	
endmodule

`endif
