`ifndef _WRREG_INCLUDED_
`define _WRREG_INCLUDED_

`include "../74xxx/octRegister_74273.v"
`include "../74xxx/HexInverter_7404.v"

module rwRegister(clock, notReset, wr, rd, notWR, notRD);
	input clock, notReset, wr, rd;
	wire clock, notReset, wr, rd;
	
	output notWR, notRD;
	wire notWR, notRD;
	
	wire [7:0] register_out;
	octRegister_74273 register(inverter_out[2], notReset, {6'bxxxxxx, wr, rd}, register_out);
	
	wire [5:0] inverter_out;
	assign notRD = inverter_out[0];
	assign notWR = inverter_out[1];
	HexInverter_7404 inverter({3'bxxx, clock, register_out[1], register_out[0]}, inverter_out);

endmodule


`endif
