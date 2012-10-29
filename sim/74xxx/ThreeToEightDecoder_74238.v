`ifndef THREE_TO_EIGHT_DECODER_74238
`define THREE_TO_EIGHT_DECODER_74238

module ThreeToEightDecoder_74238(notOE1, notOE2, OE3, in, out);
	input notOE1, notOE2, OE3;
	input in;
	output out;

	assign #32 out = ~notOE1 & ~notOE2 & OE3 ? 1 << in : 8'bzzzzzzzz;
endmodule

`endif

