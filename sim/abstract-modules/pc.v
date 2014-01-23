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
	
	output [DATA_WIDTH-1:0] out;
	
	reg [DATA_WIDTH-1:0] data;

	assign out = ~notOE ? $unsigned(data) << 1 : 32'bZ;
	
	always @ (posedge clock) begin
		if(inc) begin
			data = data + 1;
		end
		
		if(!notReset && !notLoad) begin
			data = 32'bX;
		end
		else begin
			if(!notReset) begin
				data = 0;
			end
			else if(!notLoad) begin
				data = {in[0], in[15:1]};
			end
		end
	end
	
	//generic register interface so higher level modules can monitor the content of this register
	wire [DATA_WIDTH-1:0] content = data << 1;
	
endmodule

`endif

