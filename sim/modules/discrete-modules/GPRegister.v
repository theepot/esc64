`ifndef _GPREGISTER_S_INCLUDED_
`define _GPREGISTER_S_INCLUDED_
`include "../74xxx/octRegister_74377.v"
`include "../74xxx/OctThreeState_74541.v"

module GPRegister(clock, notLoad, notOE, in, out);
	input clock, notOE, notLoad;
	input [15:0] in;
	output [15:0] out;
	
	wire [7:0] outL, outH;
	
	octRegister_74377 regL(clock, notLoad, in[7:0], outL);
	octRegister_74377 regH(clock, notLoad, in[15:8], outH);
	
	OctThreeState_74541 bufL(notOE, 0, outL , out[7:0]);
	OctThreeState_74541 bufH(notOE, 0, outH , out[15:8]);
	
	//generic register interface so higher level modules can monitor the content of this register
	wire [15:0] content = {regH.out, regL.out};
	
endmodule

`endif
