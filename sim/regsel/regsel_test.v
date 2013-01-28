`ifdef REGSEL_STRUCT
`define WAVENAME "regsel_s.vcd"
`include "regsel_s.v"
`else
`define WAVENAME "regsel.vcd"
`include "regsel.v"
`endif

`include "../globals.v"

`define TEST_DELAY (500)
`define TEST_CLOCK (800)


module regSel_test();
	wire [7:0] regNotOEs, regNotLoads;
	reg oe, load;
	reg [1:0] oeSourceSel;
	reg loadSourceSel;
	reg [2:0] useqRegSelOE, useqRegSelLoad, op0, op1, op2;

	reg[7:0] expected_regNotOEs;
	reg[7:0] expected_regNotLoads;

	task test;
	begin
		if(oe) begin
			case(oeSourceSel)
				2'b00: expected_regNotOEs = ~(1 << (useqRegSelOE));
				2'b01: expected_regNotOEs = ~(1 << (op0));
				2'b10: expected_regNotOEs = ~(1 << (op1));
				2'b11: expected_regNotOEs = ~(1 << (op2));
			endcase
		end
		else begin
			expected_regNotOEs = 8'HFF;
		end
	
		if(load) begin
			case(loadSourceSel)
				1'b0: expected_regNotLoads = ~(1 << (useqRegSelLoad));
				1'b1: expected_regNotLoads = ~(1 << (op0));
			endcase
		end
		else begin
			expected_regNotLoads = 8'HFF;
		end
		
		if(expected_regNotOEs !== regNotOEs) begin
			$display("ERROR: regsel: regNotOEs=%X (expected %X)", regNotOEs, expected_regNotOEs);
		end
		
		if(expected_regNotLoads !== regNotLoads) begin
			$display("ERROR: regsel: regNotLoads=%X (expected %X)", regNotLoads, expected_regNotLoads);
		end
	end
	endtask

	initial begin
		$dumpfile(`WAVENAME);
		$dumpvars(0);
		
		oe = 0;
		load = 0;
		oeSourceSel = 0;
		loadSourceSel = 0;
		useqRegSelOE = 0;
		useqRegSelLoad = 0;
		op0 = 0;
		op1 = 0;
		op2 = 0;
		
		#100
		oe = 1;
		load = 1;
		op0 = 1;
		op1 = 3;
		op2 = 7;
		useqRegSelOE = 2;
		#100 test();
		
		
		#100 oeSourceSel = 1;
		#100 test();
		#100 oeSourceSel = 2;
		#100 test();
		#100 oeSourceSel = 3;
		#100 test();
		#100 oeSourceSel = 0;
		#100 test();
		
		#100 loadSourceSel = 1;
		#100 test();
		
		#100
		oe = 0;
		load = 0;
		#100 test();
		
		#100 $finish;
	end
	
	regSel register_selecter(oe, load, oeSourceSel,loadSourceSel,useqRegSelOE,useqRegSelLoad,op0,op1,op2, regNotOEs, regNotLoads);

endmodule
