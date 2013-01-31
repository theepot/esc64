`ifdef STATUS_STRUCT
`include "status_s.v"
`define DUMPNAME "status_s.vcd"
`else
`include "status.v"
`define DUMPNAME "status.vcd"
`endif

`include "../globals.v"

`define TEST_DELAY (500)

module status_test();
	reg clock, cIn, zIn, notLoad;
	wire cOut, zOut;
	
	initial begin
		$dumpfile(`DUMPNAME);
		$dumpvars(0);
	
		clock = 0;
		notLoad = 0;
		cIn = 0;
		zIn = 0;
		
		cIn = 1;
		zIn = 1;
		# `TEST_DELAY clock = 1;
		# `TEST_DELAY clock = 0;
		
		# `TEST_DELAY if(cOut !== 1) begin
			$display("ERROR: status.0: cOut=%X (should be 1)", cOut);
		end
		if(zOut !== 1) begin
			$display("ERROR: status.1: zOut=%X (should be 1)", zOut);
		end
		
		# `TEST_DELAY notLoad = 1;
		cIn = 0;
		zIn = 0;
		# `TEST_DELAY clock = 1;
		# `TEST_DELAY clock = 0;
		
		# `TEST_DELAY if(cOut !== 1) begin
			$display("ERROR: status.2: cOut=%X (should be 1)", cOut);
		end
		if(zOut !== 1) begin
			$display("ERROR: status.3: zOut=%X (should be 1)", zOut);
		end
		
		# `TEST_DELAY notLoad = 0;
		# `TEST_DELAY clock = 1;
		# `TEST_DELAY clock = 0;
		
		# `TEST_DELAY if(cOut !== 0) begin
			$display("ERROR: status.4: cOut=%X (should be 0)", cOut);
		end
		if(zOut !== 0) begin
			$display("ERROR: status.5: zOut=%X (should be 0)", zOut);
		end
		
		#20	$finish;
	end
	
	statusRegister status(clock, notLoad, cIn, cOut, zIn, zOut);

endmodule

