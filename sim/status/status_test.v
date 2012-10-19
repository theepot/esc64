`include "status.v"

module status_test();
	reg clock, cIn, zIn, notLoad;
	wire cOut, zOut;
	
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0);
	
		clock = 0;
		notLoad = 0;
		cIn = 0;
		zIn = 0;
		
		#12 cIn = 1;
		#10 zIn = 1;
		
		#10 notLoad = 1;
		#2 cIn = 0;
		zIn = 0;
		
		#20 notLoad = 0;
		
		#20 $finish;
	end
	
	always begin
		#5 clock = ~clock;
	end
	
	statusRegister status(clock, notLoad, cIn, cOut, zIn, zOut);

endmodule

