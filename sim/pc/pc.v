`ifndef _PC_INCLUDED_
`define _PC_INCLUDED_

module program_counter(clk, notClr, notWrite, read, inc, in, out);
	parameter DATA_WIDTH = 16;

	input clk;
	input notClr;
	input notWrite;
	input read;
	input inc;
	input [DATA_WIDTH-1:0] in;
	wire clk;
	wire notClr;
	wire notWrite;
	wire read;
	wire inc;
	wire [DATA_WIDTH-1:0] in;
	
	output [DATA_WIDTH-1:0] out;
	wire [DATA_WIDTH-1:0] out;
	
	reg [DATA_WIDTH-1:0] data;

	assign out = read ? data : 32'bzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz;
	
	always @ (posedge clk) begin
		/*if(~(~notClr ^ ~notWrite ^ read ^ inc)) begin
			$display("OH SHI...");
			$finish;
		end*/
	
		if(!notClr) begin
			data = 0;
		end
		else if(!notWrite) begin
			data = in;
		end
		else if(inc) begin
			data = data + 1;
		end
	end
	
endmodule

`endif

