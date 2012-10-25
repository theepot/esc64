`ifndef _PC_INCLUDED_
`define _PC_INCLUDED_

module program_counter(clock, notReset, notLoad, notOE, inc, in, out);
	parameter DATA_WIDTH = 16;

	input clock;
	input notReset;
	input notLoad;
	input notOE;
	input inc;
	input [DATA_WIDTH-1:0] in;
	wire clock;
	wire notReset;
	wire notLoad;
	wire notOE;
	wire inc;
	wire [DATA_WIDTH-1:0] in;
	
	output [DATA_WIDTH-1:0] out;
	wire [DATA_WIDTH-1:0] out;
	
	reg [DATA_WIDTH-1:0] data;

	assign #45 out = ~notOE ? data : 32'bzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz;
	
	always @ (posedge clock) begin
		if(inc) begin
			data = data + 1;
		end
		
		if(!notReset && !notLoad) begin
			data = 32'bxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx;
		end
		else begin
			if(!notReset) begin
				data = 0;
			end
			else if(!notLoad) begin
				data = in;
			end
		end
		
		
	end
	
endmodule

`endif

