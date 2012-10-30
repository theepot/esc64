`ifndef THREE_TO_EIGHT_DECODER_74138
`define THREE_TO_EIGHT_DECODER_74138

module ThreeToEightDecoder_74138(notOE1, notOE2, OE3, in, notOut);
	input notOE1, notOE2, OE3;
	input[2:0] in;
	output[7:0] notOut;

	assign #11 notOut = ~notOE1 & ~notOE2 & OE3 ? ~(1 << in) : 8'b11111111;
endmodule

`endif

