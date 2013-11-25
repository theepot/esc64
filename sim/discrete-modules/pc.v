`ifndef _PC_S_INCLUDED_
`define _PC_S_INCLUDED_

`include "QuadCounter_74163.v"
`include "OctThreeState_74541.v"

module program_counter(clock, notReset, notLoad, notOE, inc, in, out);
	input clock;
	input notReset;
	input notLoad;
	input notOE;
	input inc;
	input [15:0] in;
//	wire clock;
//	wire notReset;
//	wire notLoad;
//	wire notOE;
//	wire inc;
//	wire [15:0] in;
	
	output [15:0] out;
//	wire [15:0] out;
	
	wire carry0, carry1, carry2;
	wire[15:0] counterOut;
	
	//counters
	QuadCounter_74163 counter0(clock, notReset, notLoad, inc, inc,		in[3:0],	counterOut[3:0], carry0);	//least significant
	QuadCounter_74163 counter1(clock, notReset, notLoad, inc, carry0,	in[7:4],	counterOut[7:4], carry1);
	QuadCounter_74163 counter2(clock, notReset, notLoad, inc, carry0&carry1,	in[11:8],	counterOut[11:8], carry2);
	QuadCounter_74163 counter3(clock, notReset, notLoad, inc, carry0&carry1&carry2,	in[15:12],	counterOut[15:12], /*NC*/);	//most significant

	//three-state buffers
	OctThreeState_74541 threeStateL(0, notOE, counterOut[15:8], out[15:8]);
	OctThreeState_74541 threeStateH(0, notOE, counterOut[7:0], out[7:0]);
	
	//generic register interface so higher level modules can monitor the content of this register
	wire [15:0] content = {counter3.out, counter2.out, counter1.out, counter0.out};
	
endmodule

`endif








