`ifndef _CONTROLUNIT_INCLUDED_
`define _CONTROLUNIT_INCLUDED_

`include "octRegister_74273.v"
`include "Quad2To1Mux_74157.v"
`include "rom_2Kx8.v"

`define ROM_CHIPS 7

module control_unit(clock, notReset, opcode, carry, zero, cpu_inspect, control);
	parameter ROM_FILENAME="urom.lst";
	
	input clock, notReset, carry, zero, cpu_inspect;
	input [6:0] opcode;
	output [(8*`ROM_CHIPS)-14-1:0] control;

	wire [15:0] roms_addr;
	wire [(8*`ROM_CHIPS)-1:0] roms_data;

	wire rom_next_sel = roms_data[0];
	wire [12:0] rom_next_addr = roms_data[13:1];
	assign control = roms_data[(8*`ROM_CHIPS)-1:14];
	
	rom_2Kx8 rom0(roms_addr[12:0], roms_data[7:0], 0, 0); //LSBs
	rom_2Kx8 rom1(roms_addr[12:0], roms_data[15:8], 0, 0);
	rom_2Kx8 rom2(roms_addr[12:0], roms_data[23:16], 0, 0);
	rom_2Kx8 rom3(roms_addr[12:0], roms_data[31:24], 0, 0);
	rom_2Kx8 rom4(roms_addr[12:0], roms_data[39:32], 0, 0);
	rom_2Kx8 rom5(roms_addr[12:0], roms_data[47:40], 0, 0);
	rom_2Kx8 rom6(roms_addr[12:0], roms_data[55:48], 0, 0);
	
	octRegister_74273 addr_reg0(clock, notReset, addr_mux_out[7:0],   roms_addr[7:0]); //LSBs
	octRegister_74273 addr_reg1(clock, notReset, addr_mux_out[15:8],  roms_addr[15:8]);
	
	wire [15:0] addr_mux_out;
	Quad2To1Mux_74157 addr_mux0(0, rom_next_sel, rom_next_addr[3:0],         {opcode[1:0],carry, zero}, addr_mux_out[3:0]); //LSBs
	Quad2To1Mux_74157 addr_mux1(0, rom_next_sel, rom_next_addr[7:4],          opcode[5:2],              addr_mux_out[7:4]);
	Quad2To1Mux_74157 addr_mux2(0, rom_next_sel, rom_next_addr[11:8],        {2'b00, cpu_inspect, opcode[6]},       addr_mux_out[11:8]);
	Quad2To1Mux_74157 addr_mux3(0, rom_next_sel, {3'bxxx, rom_next_addr[12]}, 4'bxxx0,                  addr_mux_out[15:12]);
	
	
	
	//load all the ROM chips with the right part from the signle rom image
	reg [(8*`ROM_CHIPS)-1:0] tmp_mem[0:(1<<13)-1];
	integer n;
	initial begin
		$readmemb(ROM_FILENAME, tmp_mem);
		n = 0;
		for(n = 0; n < (1<<13); n = n + 1) begin
			rom0.mem[n] = tmp_mem[n][7:0];
			rom1.mem[n] = tmp_mem[n][15:8];
			rom2.mem[n] = tmp_mem[n][23:16];
			rom3.mem[n] = tmp_mem[n][31:24];
			rom4.mem[n] = tmp_mem[n][39:32];
			rom5.mem[n] = tmp_mem[n][47:40];
			rom6.mem[n] = tmp_mem[n][55:48];
		end
		
	end
	
endmodule

`endif

