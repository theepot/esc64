`ifndef HEX_INVERTER_7404_INCLUDED
`define HEX_INVERTER_7404_INCLUDED

module HexInverter_7404(a, y);
    input   [5:0] a;
    output  [5:0] y;
    
    assign #(17+15) y = ~a;
endmodule

`endif

