`ifndef _IO_INTERFACE_INCLUDED_
`define _IO_INTERFACE_INCLUDED_

`include "HexInverter_7404.v"
`include "octRegister_74377.v"
`include "octRegister_74273.v"
`include "OctThreeState_74541.v"
`include "Quad2InputNand_7400.v"
`include "OctBusTransceiver_74245.v"

/*
	7404		
	7402		
	74273		
	74377		
	74541		
	total		
*/

//TODO: change direction of bus transceivers, and ivert dir_out, so we can use one inverter less

module io_interface(clock, notReset, a_bus, y_bus, rd, wr, select_dev, idle_n, word, dir_out, address_ld_n, data_ld_n, out_address, inout_data, out_rd_n, out_wr_n, out_csl_n, out_csh_n, out_select_dev);
	input clock, notReset, rd, wr, select_dev, idle_n, word, dir_out, address_ld_n, data_ld_n;
	inout [15:0] a_bus;
	inout [15:0] y_bus;
	output out_rd_n, out_wr_n, out_csl_n, out_csh_n, out_select_dev;
	inout [14:0] out_address;
	inout [15:0] inout_data;
	
	wire clock, notReset, rd, wr, select_dev, out_rd_n, out_wr_n, out_csl_n, out_csh_n, out_select_dev, idle_n, word, dir_out, address_ld_n, data_ld_n;
	wire [15:0] a_bus;
	wire [15:0] y_bus;
	wire [14:0] out_address;
	wire [15:0] inout_data;
	
	
	//----------data reg and drivers
	wire [7:0] data_out_reg_out_low;
	wire [7:0] data_out_reg_out_high;
	wire [7:0] data_internal_bus_low;
	wire [7:0] data_internal_bus_high;
	wire data_out_driver_oe_n = dir_out_clean_n;
	wire data_in_driver_oe_n;
	octRegister_74377 data_out_reg_low(clock, data_ld_n, y_bus[7:0], data_out_reg_out_low);									//U1	board1
	octRegister_74377 data_out_reg_high(clock, data_ld_n, y_bus[15:8], data_out_reg_out_high);								//U2	board1
	OctThreeState_74541 data_out_driver_low(1'b0, data_out_driver_oe_n, data_out_reg_out_low, data_internal_bus_low);		//U3	board1
	OctThreeState_74541 data_out_driver_high(1'b0, data_out_driver_oe_n, data_out_reg_out_high, data_internal_bus_high);	//U4	board1
	OctThreeState_74541 data_in_driver_low(1'b0, data_in_driver_oe_n, data_internal_bus_low, y_bus[7:0]);					//U5	board1
	OctThreeState_74541 data_in_driver_high(1'b0, data_in_driver_oe_n, data_internal_bus_high, y_bus[15:8]);				//U6	board1
	//----------

	//----------address reg and drivers
	wire [15:0] address_out_reg_out;
	wire [15:0] address_out_driver_out;
	wire address_out_driver_oe_n = idle_clean;
	wire address0 = address_out_reg_out[0];
	assign out_address = address_out_driver_out[15:1];
	octRegister_74377 address_out_reg_low(clock, address_ld_n, a_bus[7:0], address_out_reg_out[7:0]);										//U7	board2
	octRegister_74377 address_out_reg_high(clock, address_ld_n, a_bus[15:8], address_out_reg_out[15:8]);									//U8	board2
	OctThreeState_74541 address_out_driver_low(1'b0, address_out_driver_oe_n, address_out_reg_out[7:0], address_out_driver_out[7:0]);		//U9	board2
	OctThreeState_74541 address_out_driver_high(1'b0, address_out_driver_oe_n, address_out_reg_out[15:8], address_out_driver_out[15:8]);	//U10	board2
	//----------

	//----------inverters
	wire [5:0] inverter1_out;
	wire dir_out_clean_n= inverter1_out[5];
	wire address0_n		= inverter1_out[4];
	wire idle_clean		= inverter1_out[3];
	assign out_wr_n 	= inverter1_out[2];
	assign out_rd_n		= inverter1_out[1];
	wire clock_n		= inverter1_out[0];
	HexInverter_7404 inverter1({					//U11	board2
		dir_out_clean,	//5
		address0,		//4
		idle_clean_n,	//3
		wr_clean,		//2
		rd_clean,		//1
		clock			//0
	}, inverter1_out);
	//----------

	//----------anti glitch
	wire [7:0] anti_glitch_register_out;
	assign out_select_dev= anti_glitch_register_out[5];
	wire dir_out_clean	= anti_glitch_register_out[4];
	wire word_clean		= anti_glitch_register_out[3];
	wire idle_clean_n	= anti_glitch_register_out[2];
	wire wr_clean		= anti_glitch_register_out[1];
	wire rd_clean		= anti_glitch_register_out[0];
	octRegister_74273 anti_glitch_register(clock_n, notReset, {		//U13
		2'bX,
		select_dev,		//5
		dir_out,		//4
		word,			//3
		idle_n,			//2
		wr,				//1
		rd				//0
	}, anti_glitch_register_out);
	//----------
	
	//----------byte bus transceivers
	wire low_to_low_transceiver_oe_n;
	wire high_to_low_transceiver_oe_n;
	wire high_to_high_transceiver_oe_n;
	wire zero_to_high_oe_n;
	OctBusTransceiver_74245 low_to_low_transceiver(low_to_low_transceiver_oe_n, dir_out_clean, data_internal_bus_low, inout_data[7:0]);			//U14	connects to board1	U3 U5
	OctBusTransceiver_74245 high_to_low_transceiver(high_to_low_transceiver_oe_n, dir_out_clean, data_internal_bus_low, inout_data[15:8]);		//U15	connects to board1	U3 U5
	OctBusTransceiver_74245 high_to_high_transceiver(high_to_high_transceiver_oe_n, dir_out_clean, data_internal_bus_high, inout_data[15:8]);	//U16	connects to board1	U4 U6
	OctThreeState_74541 zero_to_high(1'b0, zero_to_high_oe_n, 8'b0, data_internal_bus_high);													//U17	connects to board1	U4 U6
	//----------
	
	//----------nands
	wire [3:0] nand1_output;
	assign data_in_driver_oe_n				= nand1_output[3];
	assign zero_to_high_oe_n				= nand1_output[2];
	assign high_to_high_transceiver_oe_n	= nand1_output[1];
	assign high_to_low_transceiver_oe_n		= nand1_output[0];
	Quad2InputNand_7400 nand1(															//U18
	{idle_clean_n,		high_to_high_transceiver_oe_n, 	word_clean,		address0},
	{dir_out_clean_n,	dir_out_clean_n,				idle_clean_n,	idle_clean_n},
	nand1_output);
	
	wire [3:0] nand2_output;
	assign out_csh_n						= nand2_output[2];
	assign low_to_low_transceiver_oe_n		= nand2_output[1];
	wire csh								= nand2_output[0];
	Quad2InputNand_7400 nand2(															//U19
	{1'bX,	csh,	address0_n,		high_to_high_transceiver_oe_n},
	{1'bX,	csh,	idle_clean_n,	high_to_low_transceiver_oe_n},
	nand2_output);
	//----------
	
	assign out_csl_n = low_to_low_transceiver_oe_n;
	
//	TODO: make sure this does not warn us when there is nothing bad going on
//	always @ (address0, word_clean, idle_clean) begin
//		if(address0 && word_clean && !idle_clean) begin
//			$display("WARNING: trying to access unaligned word");
//		end
//	end
	
endmodule

`endif
