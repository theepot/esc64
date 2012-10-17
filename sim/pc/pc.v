`ifndef _PC_INCLUDED_
`define _PC_INCLUDED_

module program_counter(clock, notReset, notLoad, OE, inc, in, out);
	parameter DATA_WIDTH = 16;

	input clock;
	input notReset;
	input notLoad;
	input OE;
	input inc;
	input [DATA_WIDTH-1:0] in;
	wire clock;
	wire notReset;
	wire notLoad;
	wire OE;
	wire inc;
	wire [DATA_WIDTH-1:0] in;
	
	output [DATA_WIDTH-1:0] out;
	wire [DATA_WIDTH-1:0] out;
	
	reg [DATA_WIDTH-1:0] data;

	assign out = OE ? data : 32'bzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz;
	
	always @ (posedge clock) begin
		/*if(~(~notReset ^ ~notLoad ^ OE ^ inc)) begin
			$display("OH SHI...");
			$finish;
		end*/
	
		if(!notReset) begin
			data = 0;
		end
		else if(!notLoad) begin
			data = in;
		end
		else if(inc) begin
			data = data + 1;
		end
	end
	
endmodule

`endif

