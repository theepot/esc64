`ifndef _SRAM_INCLUDED_
`define _SRAM_INCLUDED_

//TODO: a write cycle should also be able to be controlled by notCS while notWE is low.

module sram(addr, data, notOE, notWE, notCS);
	parameter ADDR_WIDTH = 16;
	parameter DATA_WIDTH = 16;
	parameter MEMFILE = "";
	
	input [ADDR_WIDTH-1:0] addr;
	wire [ADDR_WIDTH-1:0] addr;
	
	inout [DATA_WIDTH-1:0] data;
	wire [DATA_WIDTH-1:0] data;
	
	input notOE, notWE, notCS;
	wire notOE, notWE, notCS;
	
	reg [DATA_WIDTH-1:0] mem[0:((1<<ADDR_WIDTH)-1)];
	
	reg [ADDR_WIDTH-1:0] addr_latch;

	assign #(55) data = (!notCS && !notOE) ? mem[addr] : 16'bzzzzzzzzzzzzzzzz;
	
	initial begin
		if(MEMFILE != "") begin
			$readmemb(MEMFILE, mem);
		end
		else begin
			$display("warning: sram: no memory file defined");
		end
		
	end

	always @ (negedge notWE) begin
		if(!notCS) begin
			addr_latch = addr;
		end
	end

	always @ (posedge notWE) begin
		if(!notCS) begin
			mem[addr_latch] = data;
		end
	end

endmodule

`endif