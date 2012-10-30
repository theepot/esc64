`ifndef _REGSEL_S_INCLUDED_
`define _REGSEL_S_INCLUDED_

`include "../74xxx/ThreeToEightDecoder_74138.v"
`include "../74xxx/Dual4To1Mux_74153.v"
`include "../74xxx/Quad2To1Mux_74157.v"

module regSel(oe, load, oeSourceSel, loadSourceSel, useqRegSelOE, useqRegSelLoad, op0, op1, op2, regNotOEs, regNotLoads);
	input oe, load;
	input [1:0] oeSourceSel;
	input loadSourceSel;
	input [2:0] useqRegSelOE, useqRegSelLoad, op0, op1, op2;
	output [7:0] regNotOEs, regNotLoads;
	
	wire oe, load;
	wire [1:0] oeSourceSel;
	wire loadSourceSel;
	wire [2:0] useqRegSelOE, useqRegSelLoad, op0, op1, op2;
	//reg [7:0] regNotOEs, regNotLoads;
	
	//decoders
	wire[2:0] OEDecoderIn;
	ThreeToEightDecoder_74138 OEDecoder(0, 0, oe, OEDecoderIn, regNotOEs);
	
	wire[2:0] loadDecoderIn;
	ThreeToEightDecoder_74138 loadDecoder(0, 0, load, loadDecoderIn, regNotLoads);

	//OE multiplexers	
	wire[1:0] OEMux1Out;
	Dual4To1Mux_74153 OEMux1(
		0, oeSourceSel,
		 {1'bx, useqRegSelOE[0]}, {1'bx, op0[0]}, {1'bx, op1[0]}, {1'bx, op2[0]},
		OEMux1Out);
	
	wire[1:0] OEMux0Out;
	Dual4To1Mux_74153 OEMux0(
		0, oeSourceSel, 
		{useqRegSelOE[2], useqRegSelOE[1]}, {op0[2], op0[1]}, {op1[2], op1[1]}, {op2[2], op2[1]},
		OEMux0Out);
	
	assign OEDecoderIn[2:1] = OEMux0Out[1:0];
	assign OEDecoderIn[0] = OEMux1Out[0];
	 
	//load multiplexer
	wire[3:0] loadMuxOut;
	Quad2To1Mux_74157 loadMux(
		0, loadSourceSel,
		{1'bx, useqRegSelLoad[2], useqRegSelLoad[1], useqRegSelLoad[0]}, {1'bx, op0[2], op0[1], op0[0]},
		loadMuxOut);
	
	assign loadDecoderIn = loadMuxOut[2:0];

	//DEBUG
	/*always @ (oeSourceSel) begin $display("oeSourceSel=%X", oeSourceSel); end
	always @ (OEMux0Out or OEMux1Out) begin $display("OEMux0Out=%X; OEMux1Out=%X; OEDecuderIn=%X", OEMux0Out, OEMux1Out, OEDecoderIn); end
	always @ (loadMuxOut) begin $display("loadMuxOut=%X; loadDecoderIn=%X", loadMuxOut, loadDecoderIn); end
	always @ (regNotOEs) begin $display("regNotOEs=%X", regNotOEs); end
	always @ (regNotLoads) begin $display("regNotLoads=%X", regNotLoads); end*/

endmodule

`endif

