//chip code:            7404
//chip description:     hex inverting gate
//author:               kasper
//version:              1
//comment:
//TODO:					fix delay

`ifndef HEX_INVERTOR_INCLUDED
`define HEX_INVERTOR_INCLUDED

module ic7404(a, y);
    input   [5:0] a;
    output  [5:0] y;
    
    assign #5 y = ~a;
endmodule

`endif

