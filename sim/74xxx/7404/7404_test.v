`include "7404.v"
module ic7404_test();
    reg [5:0] a;
    wire [5:0] y;
    
    initial begin
        $monitor("time: %d | a:%d y:%d | a:%d y:%d | a:%d y:%d | a:%d y:%d | a:%d y:%d | a:%d y:%d", $time, a[0], y[0], a[1], y[1], a[2], y[2], a[3], y[3], a[4], y[4], a[5], y[5]);
        a[0] = 0;
        a[1] = 0;
        a[2] = 0;
        a[3] = 0;
        a[4] = 0;
        a[5] = 0;
        
        #10 a[0] = 1;
        a[1] = 1;
        a[2] = 1;
        a[3] = 1;
        a[4] = 1;
        a[5] = 1;
        
        
        #10 $finish;
    end
    
    ic7404 subject(a, y);

endmodule
