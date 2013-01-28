`ifndef _QUAD2INPUTAND_H_
`define _QUAD2INPUTAND_H_

module Quad2InputAnd_7408(a, b, y);
    input   [3:0] a;
    input   [3:0] b;
    output  [3:0] y;
    
    assign #(18+15) y = a & b;
endmodule

`endif
