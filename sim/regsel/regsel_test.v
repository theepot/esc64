`include "regsel.v"
<<<<<<< Updated upstream
=======
`include "../globals.v"
>>>>>>> Stashed changes

module regSel_test();
	wire [7:0] regOes, regNotLoads;
	reg oe, load;
	reg [1:0] oeSourceSel;
	reg loadSourceSel;
	reg [2:0] useqRegSelOe, useqRegSelLoad, op0, op1, op2;
	

	reg[7:0] expected_regOes;
	reg[7:0] expected_regNotLoads;

	task test;
	begin
		if(oe) begin
			case(oeSourceSel)
				2'b00: expected_regOes = 1 << (useqRegSelOe);
				2'b01: expected_regOes = 1 << (op0);
				2'b10: expected_regOes = 1 << (op1);
				2'b11: expected_regOes = 1 << (op2);
			endcase
		end
		else begin
			expected_regOes = 8'H00;
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
		
		if(expected_regOes !== regOes) begin
			$display("ERROR: regsel: regOes=%X (expected %X)", regOes, expected_regOes);
		end
		
		if(expected_regNotLoads !== regNotLoads) begin
			$display("ERROR: regsel: regNotLoads=%X (expected %X)", regNotLoads, expected_regNotLoads);
		end
	end
	endtask

	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0, regOes, regNotLoads, oe, load, oeSourceSel, loadSourceSel, useqRegSelOe, useqRegSelLoad, op0, op1, op2);
		
		oe = 0;
		load = 0;
		oeSourceSel = 0;
		loadSourceSel = 0;
		useqRegSelOe = 0;
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
		useqRegSelOe = 2;
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
	
	regSel register_selecter(oe, load, oeSourceSel,loadSourceSel,useqRegSelOe,useqRegSelLoad,op0,op1,op2, regOes, regNotLoads);

endmodule
