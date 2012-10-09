`include "7400.v"
module ic7400_test();
    reg [3:0] a;
    reg [3:0] b;
    wire [3:0] y;
    
    initial begin
        $monitor("time: %d | a:%d b:%d y:%d | a:%d b:%d y:%d | a:%d b:%d y:%d | a:%d b:%d y:%d", $time, a[0], b[0], y[0], a[1], b[1], y[1], a[2], b[2], y[2], a[3], b[3], y[3]);
        a[0] = 0;
        a[1] = 0;
        a[2] = 0;
        a[3] = 0;
        
        b[0] = 0;
        b[1] = 0;
        b[2] = 0;
        b[3] = 0;
        
        #10 a[0] = 1;
        b[0] = 1;
        
        #10 a[1] = 1;
        b[1] = 1;
        
        #10 a[2] = 1;
        b[2] = 1;
        
        #10 a[3] = 1;
        b[3] = 1;
        
        #10 $finish;
    end
    
    ic7400 subject(a, b, y);

endmodule
