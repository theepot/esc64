`ifndef _QUAD2INPUTXOR_H_
`define _QUAD2INPUTXOR_H_

module Quad2InputXor_7486(a, b, y);
    input   [3:0] a;
    input   [3:0] b;
    output  [3:0] y;
    
    assign #(24+15) y = a ^ b;
endmodule

`endif
