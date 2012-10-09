module rom(addr, data);
	parameter MEMFILE = "rom.lst";
	parameter DATA_WIDTH = 8;
	parameter ADDR_WIDTH = 4;
	input [ADDR_WIDTH-1:0] addr;
	output [ADDR_WIDTH-1:0] data;
	wire addr;
	reg [DATA_WIDTH-1:0] mem[0:((1<<ADDR_WIDTH)-1)];
	reg [DATA_WIDTH-1:0] data;

	initial begin
		$readmemb(MEMFILE, mem);
	end

	always @ (addr)
	begin
		data <= mem[addr];
	end

endmodule
