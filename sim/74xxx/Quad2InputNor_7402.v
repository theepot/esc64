//chip code:            7402
//chip description:     quad 2-input nor gate
//author:               kasper
//version:              1
//comment:
//TODO:					included guards, delay, 1 assignment

module ic7402(a, b, y);
    input   [3:0] a;
    input   [3:0] b;
    output  [3:0] y;
    
    assign #5 y[0] = ~(a[0] | b[0]);
    assign #5 y[1] = ~(a[1] | b[1]);
    assign #5 y[2] = ~(a[2] | b[2]);
    assign #5 y[3] = ~(a[3] | b[3]);
    
endmodule
