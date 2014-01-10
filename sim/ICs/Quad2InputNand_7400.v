`ifndef _QUAD2INPUTNAND_7400_H_
`define _QUAD2INPUTNAND_7400_H_

module Quad2InputNand_7400(a, b, y);
	input   [3:0] a;
	input   [3:0] b;
	output  [3:0] y;

	assign #(9+7) y = ~(a & b);
endmodule

`endif
