`ifndef ROM_2KX8_INCLUDED
`define ROM_2KX8_INCLUDED

module rom_2Kx8(addr, data, notCE, notOE);
	parameter MEMFILE = "rom.lst";
	input notCE, notOE;
	input [12:0] addr;
	output [7:0] data;
	
	wire [12:0] addr;
	wire [7:0] data;

	reg [7:0] mem[0:1<<13];
	
	assign #150 data = (~notCE && ~notOE) ? mem[addr] : 16'HZZZZ;
	
	initial begin
		//$readmemb(MEMFILE, mem);
	end
	
endmodule

`endif
