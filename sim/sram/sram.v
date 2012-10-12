`ifndef _SRAM_INCLUDED_
`define _SRAM_INCLUDED_

module sram(addr, data, notOE, notWE, notCS);
	parameter ADDR_WIDTH = 16;
	parameter DATA_WIDTH = 16;
	
	input [ADDR_WIDTH-1:0] addr;
	wire [ADDR_WIDTH-1:0] addr;
	
	inout [DATA_WIDTH-1:0] data;
	wire [DATA_WIDTH-1:0] data;
	
	input notOE, notWE, notCS;
	wire notOE, notWE, notCS;
	
	reg [DATA_WIDTH-1:0] mem[0:((1<<ADDR_WIDTH)-1)];

	assign data = !notCS && !notOE ? mem[addr] : 16'bzzzzzzzzzzzzzzzz;

	always @ (posedge notWE) begin
		if(!notCS && notOE) begin
			mem[addr] = data;
		end
	end

endmodule

`endif