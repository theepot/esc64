`ifndef QUAD2INPUTAND_H
`define QUAD2INPUTAND_H

module Quad2InputAnd_7408(a, b, y);
    input   [3:0] a;
    input   [3:0] b;
    output  [3:0] y;
    
    assign #18 y = a & b;
endmodule

`endif
