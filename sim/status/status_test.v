`ifdef STATUS_STRUCT
`include "status_s.v"
`define DUMPNAME "status_s.vcd"
`else
`include "status.v"
`define DUMPNAME "status.vcd"
`endif

`include "../globals.v"

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
		
		#100	cIn = 1;
				zIn = 1;
		#100	if(cOut !== 1) begin
			$display("ERROR: status: cOut=%X (should be 1)", cOut);
		end
		if(zOut !== 1) begin
			$display("ERROR: status: zOut=%X (should be 1)", zOut);
		end
		
		#100	notLoad = 1;
				cIn = 0;
				zIn = 0;
		#100	if(cOut !== 1) begin
			$display("ERROR: status: cOut=%X (should be 1)", cOut);
		end
		if(zOut !== 1) begin
			$display("ERROR: status: zOut=%X (should be 1)", zOut);
		end
		
		#100	notLoad = 0;
		#100	if(cOut !== 0) begin
			$display("ERROR: status: cOut=%X (should be 0)", cOut);
		end
		if(zOut !== 0) begin
			$display("ERROR: status: zOut=%X (should be 0)", zOut);
		end
		
		#20		$finish;
	end
	
	always begin
		#20		clock = ~clock;
	end
	
	statusRegister status(clock, notLoad, cIn, cOut, zIn, zOut);

endmodule

