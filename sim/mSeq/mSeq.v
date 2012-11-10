`ifndef _MSEQ_INCLUDED_
`define _MSEQ_INCLUDED_

`include "../register/register.v"

module rom(addr, data);
	parameter MEMFILE = "rom.lst";
	parameter DATA_WIDTH = 8;
	parameter ADDR_WIDTH = 4;
	input [ADDR_WIDTH-1:0] addr;
	output [DATA_WIDTH-1:0] data;
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

module mSeq(clock, notReset, opcode, carry, zero, control);
	parameter OPCODE_WIDTH = 7;
	parameter ADDR_WIDTH = 13;
	parameter CONTROL_WIDTH = 27;
	parameter INITIAL_ADDRESS = 0;
	parameter ROM_FILENAME="urom.lst";
	//`define ROM_WIDTH = (CONTROL_WIDTH + ADDR_WIDTH + 1)
	
	input clock, notReset, carry, zero;
	input [OPCODE_WIDTH-1:0] opcode;
	output [CONTROL_WIDTH-1:0] control;
	
	wire clock, notReset, carry, zero;
	wire [OPCODE_WIDTH-1:0] opcode;
	wire [CONTROL_WIDTH-1:0] control;
	
	wire [(CONTROL_WIDTH + ADDR_WIDTH + 1)-1:0] rom_data;
	wire rom_data_sel;
	wire [ADDR_WIDTH-1:0] rom_data_next;
	assign #150 rom_data_sel = rom_data[0];
	assign #150 rom_data_next = rom_data[ADDR_WIDTH:1];
	assign #150 control = rom_data[CONTROL_WIDTH + ADDR_WIDTH:ADDR_WIDTH + 1];
	
	wire [ADDR_WIDTH-1:0] address_register_output, address_register_input;
	assign address_register_input = !notReset ? INITIAL_ADDRESS : rom_data_sel ? ((opcode << 2) | (carry << 1) | zero) : rom_data_next;
	
	rom #(.MEMFILE(ROM_FILENAME), .DATA_WIDTH((CONTROL_WIDTH + ADDR_WIDTH + 1)), .ADDR_WIDTH(ADDR_WIDTH)) mem(address_register_output, rom_data);
	
	register #(.DATA_WIDTH(ADDR_WIDTH)) address_register(clock, 1, 0, 1, address_register_input, address_register_output);
endmodule

`endif

