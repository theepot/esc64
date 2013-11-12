`ifndef _GPREGISTER_INCLUDED_
`define _GPREGISTER_INCLUDED_

module GPRegister(clock, notLoad, notOE, in, out);
	parameter DATA_WIDTH = 16;
	input clock, notOE, notLoad;
	input [DATA_WIDTH-1:0] in;
	output [DATA_WIDTH-1:0] out;
	
	wire clock, notOE, notLoad;
	wire [DATA_WIDTH-1:0] in;
	wire [DATA_WIDTH-1:0] out;
	
	reg [DATA_WIDTH-1:0] data;
	
	assign out = (~notOE) ? data : 32'bzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz;
	
	always @ (posedge clock)
	begin
		if(!notLoad) begin
			data = in;
		end
	end
	
	//generic register interface so higher level modules can monitor the content of this register
	wire [DATA_WIDTH-1:0] content = data;
	
endmodule

`endif
