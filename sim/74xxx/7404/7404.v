//chip code:            7404
//chip description:     hex inverting gate
//author:               kasper
//version:              1
//comment:

module ic7404(a, y);
    input   [5:0] a;
    output  [5:0] y;
    
    assign #5 y[0] = ~a[0];
    assign #5 y[1] = ~a[1];
    assign #5 y[2] = ~a[2];
    assign #5 y[3] = ~a[3];
    assign #5 y[4] = ~a[4];
    assign #5 y[5] = ~a[5];
    
endmodule
