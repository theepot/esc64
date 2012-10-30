/****************************************************************************
 *                                                                          *
 *  VERILOG BEHAVIORAL DESCRIPTION OF THE TI 74181 CIRCUIT                  *
 *                                                                          *
 *  Function: 4-bit ALU/Function Generator                                  *
 *                                                                          *
 *  Written by: Mark C. Hansen                                              *
 *                                                                          *
 *  Last modified: Dec 11, 1997                                             *
 *  Modified for HC timings.                                                *
 * max propagation time for 74LS181 is 62ns (for A_eq_B)                    *
 * max propagation time for 74LS181 is 41ns for Cout, when doing ripple carry
 ****************************************************************************/

module ALU_74181 (S, A, B, M, CNb, F, X, Y, CN4b, AEB);

	input [3:0] A, B, S;
	input CNb, M; 
	output [3:0] F;
	output AEB, X, Y, CN4b;
	
	wire [3:0] Fint;
	wire AEBint, Xint, Yint, CN4bint;
	
	TopLevel74181b Ckt74181b (S, A, B, M, CNb, Fint, Xint, Yint, CN4bint, AEBint);
	
	assign #40 CN4b = CN4bint;
	assign #55 F = Fint;
	assign #55 AEB = AEBint;
	assign #55 X = Xint;
	assign #55 Y = Yint;

endmodule

module TopLevel74181b (S, A, B, M, CNb, F, X, Y, CN4b, AEB);

	input [3:0] A, B, S;
	input CNb, M; 
	output [3:0] F;
	output AEB, X, Y, CN4b;
	wire [3:0] E, D, C, Bb;
	
	Emodule Emod1 (A, B, S, E);
	Dmodule Dmod2 (A, B, S, D);
	CLAmodule CLAmod3(E, D, CNb, C, X, Y, CN4b);
	Summodule Summod4(E, D, C, M, F, AEB);

endmodule

module Emodule (A, B, S, E);

	input [3:0] A, B, S;
	output [3:0] E;
	wire [3:0]  ABS3, ABbS2;

	assign ABS3 = A&B&{4{S[3]}};
	assign ABbS2 = A&~B&{4{S[2]}};
	assign E = ~(ABS3|ABbS2);

endmodule

module Dmodule (A, B, S, D);

	input [3:0] A, B, S;
	output [3:0] D;
	wire [3:0]  BbS1, BS0;  

	assign BbS1 = ~B&{4{S[1]}};
	assign BS0 = B&{4{S[0]}};
	assign D = ~(BbS1|BS0|A);

endmodule

module CLAmodule(Gb, Pb, CNb, C, X, Y, CN4b);

	input [3:0] Gb, Pb;
	input CNb; 
	output [3:0] C;
	output X, Y, CN4b;

	assign C[0] = ~CNb;
	assign C[1] = ~(Pb[0]|(CNb&Gb[0]));
	assign C[2] = ~(Pb[1]|(Pb[0]&Gb[1])|(CNb&Gb[0]&Gb[1]));
	assign C[3] = ~(Pb[2]|(Pb[1]&Gb[2])|(Pb[0]&Gb[1]&Gb[2])|(CNb&Gb[0]&Gb[1]&Gb[2]));
	assign X = ~&Gb;
	assign Y = ~(Pb[3]|(Pb[2]&Gb[3])|(Pb[1]&Gb[2]&Gb[3])|(Pb[0]&Gb[1]&Gb[2]&Gb[3]));
	assign CN4b = ~(Y&~(&Gb&CNb));

endmodule

module Summodule(E, D, C, M, F, AEB);

	input [3:0] E, D, C;
	input M; 
	output [3:0] F;
	output AEB;

	assign F = (E ^ D) ^ (C|{4{M}});
	assign AEB = &F;

endmodule
