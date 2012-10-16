`ifndef _MSEQ_INCLUDED_
`define _MSEQ_INCLUDED_

`include "../rom/rom.v"
`include "../register/register.v"

module mSeq(clock, reset, opcode, carry, zero, control);
	parameter OPCODE_WIDTH = 7;
	parameter ADDR_WIDTH = 8;
	parameter CONTROL_WIDTH = 21;
	parameter INITIAL_ADDRESS = 0;
	parameter ROM_FILENAME="urom.lst";
	//`define ROM_WIDTH = (CONTROL_WIDTH + ADDR_WIDTH + 1)
	
	input clock, reset, carry, zero;
	input [OPCODE_WIDTH-1:0] opcode;
	output [CONTROL_WIDTH-1:0] control;
	
	wire clock, reset, carry, zero;
	wire [OPCODE_WIDTH-1:0] opcode;
	wire [CONTROL_WIDTH-1:0] control;
	
	wire [(CONTROL_WIDTH + ADDR_WIDTH + 1)-1:0] rom_data;
	wire rom_data_sel;
	wire [ADDR_WIDTH-1:0] rom_data_next;
	assign rom_data_sel = rom_data[0];
	assign rom_data_next = rom_data[ADDR_WIDTH:1];
	assign control = rom_data[CONTROL_WIDTH + ADDR_WIDTH:ADDR_WIDTH + 1];
	
	wire [ADDR_WIDTH-1:0] addres_register_output, addres_register_input;
	assign addres_register_input = reset ? INITIAL_ADDRESS : rom_data_sel ? rom_data_next : (opcode | (zero << OPCODE_WIDTH) | (carry << (OPCODE_WIDTH + 1)));
	
	rom #(.MEMFILE(ROM_FILENAME), .DATA_WIDTH((CONTROL_WIDTH + ADDR_WIDTH + 1)), .ADDR_WIDTH(ADDR_WIDTH)) mem(addres_register_output, rom_data);
	
	register #(.DATA_WIDTH(ADDR_WIDTH)) addres_register(clock, 0, 1, 1, addres_register_input, addres_register_output);
endmodule

`endif

