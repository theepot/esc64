`ifndef _IO_INTERFACE_INCLUDED_
`define _IO_INTERFACE_INCLUDED_

`include "HexInverter_7404.v"
`include "octRegister_74377.v"
`include "octRegister_74273.v"
`include "OctThreeState_74541.v"
`include "Quad2InputNor_7402.v"

/*
	7404		2
	7402		1
	74273		1
	74377		4
	74541		6
	total		14 :o oh noes
*/

module io_interface(clock, notReset, a_bus, y_bus, rd, wr, data_dir_in, data_dir_out, address_ld_n, data_ld_n, out_address, inout_data, out_rd_n, out_wr_n);
	input clock, notReset, rd, wr, data_dir_in, data_dir_out, address_ld_n, data_ld_n;
	inout [15:0] a_bus;
	inout [15:0] y_bus;
	output out_rd_n, out_wr_n;
	inout [15:0] out_address;
	inout [15:0] inout_data;
	
	wire clock, notReset, rd, wr, out_rd_n, out_wr_n, data_dir_in, data_dir_out, address_ld_n, data_ld_n;
	wire [15:0] a_bus;
	wire [15:0] y_bus;
	wire [15:0] out_address;
	wire [15:0] inout_data;
	
	wire [5:0] inverter1_out;
	assign out_rd_n = inverter1_out[1];
	assign out_wr_n = inverter1_out[2];
	HexInverter_7404 inverter1({data_dir_in, anti_glitch_register_out[3], anti_glitch_register_out[2], anti_glitch_register_out[1], anti_glitch_register_out[0], clock}, inverter1_out);
	
	wire [5:0] inverter2_out;
	HexInverter_7404 inverter2({5'bX, nor_out[0]}, inverter2_out);
	
	wire [3:0] nor_out;
	Quad2InputNor_7402 nor1({3'bX, data_dir_in}, {3'bX, data_dir_out}, nor_out);
	
	wire [7:0] anti_glitch_register_out;
	octRegister_74273 anti_glitch_register(inverter1_out[0], notReset, {4'bxxxx, inverter2_out[0], data_dir_out, wr, rd}, anti_glitch_register_out);
	
	wire [0:7] address_out_reg_out_low;
	octRegister_74377 address_out_reg_low(clock, address_ld_n, a_bus[7:0], address_out_reg_out_low);
	wire [0:7] address_out_reg_out_high;
	octRegister_74377 address_out_reg_high(clock, address_ld_n, a_bus[15:8], address_out_reg_out_high);
	OctThreeState_74541 address_out_driver_low(1'b0, inverter1_out[4], address_out_reg_out_low, out_address[7:0]);
	OctThreeState_74541 address_out_driver_high(1'b0, inverter1_out[4], address_out_reg_out_high, out_address[15:8]);
	
	wire [0:7] data_out_reg_out_low;
	octRegister_74377 data_out_reg_low(clock, data_ld_n, y_bus[7:0], data_out_reg_out_low);
	wire [0:7] data_out_reg_out_high;
	octRegister_74377 data_out_reg_high(clock, data_ld_n, y_bus[15:8], data_out_reg_out_high);
	OctThreeState_74541 data_out_driver_low(1'b0, inverter1_out[3], data_out_reg_out_low, inout_data[7:0]);
	OctThreeState_74541 data_out_driver_high(1'b0, inverter1_out[3], data_out_reg_out_high, inout_data[15:8]);
	
	OctThreeState_74541 data_in_driver_low(1'b0, inverter1_out[5], inout_data[7:0], y_bus[7:0]);
	OctThreeState_74541 data_in_driver_high(1'b0, inverter1_out[5], inout_data[15:8], y_bus[15:8]);
	
	
endmodule

`endif
