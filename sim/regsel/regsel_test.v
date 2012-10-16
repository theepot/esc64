`include "regsel.v"
module regSel_test();
	wire [7:0] regOes, regLoads;
	reg oe, load;
	reg [1:0] oeSourceSel;
	reg loadSourceSel;
	reg [2:0] useqRegSelOe, useqRegSelLoad, op0, op1, op2;
	
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0, regOes, regLoads, oe, load, oeSourceSel, loadSourceSel, useqRegSelOe, useqRegSelLoad, op0, op1, op2);
		
		oe = 0;
		load = 0;
		oeSourceSel = 0;
		loadSourceSel = 0;
		useqRegSelOe = 0;
		useqRegSelLoad = 0;
		op0 = 0;
		op1 = 0;
		op2 = 0;
		
		#10 oe = 1;
		load = 1;
		op0 = 1;
		op1 = 3;
		op2 = 7;
		useqRegSelOe = 2;
		
		#10 oeSourceSel = 1;
		#10 oeSourceSel = 2;
		#10 oeSourceSel = 3;
		#10 oeSourceSel = 0;
		
		#10 loadSourceSel = 1;
		
		#10 oe = 0;
		load = 0;
		
		#10 $finish;
	end
	
	regSel register_selecter(oe, load, oeSourceSel,loadSourceSel,useqRegSelOe,useqRegSelLoad,op0,op1,op2, regOes, regLoads);

endmodule
