`include "sram.v"

module sram_test();
	reg [3:0] addr;
	
	reg [3:0] dataReg;
	wire [3:0] dataBus;
	reg dataBusOE;
	assign dataBus = dataBusOE ? dataReg : 16'bzzzzzzzzzzzzzzzz;
	
	reg notOE;
	reg notWE;
	reg notCS;
	
	integer i;
	
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0, addr, dataBus, notOE, notWE, notCS);
		
		addr = 0;
		dataReg = 0;
		dataBusOE = 0;
		notOE = 1;
		notWE = 1;
		notCS = 1;
		#1 notCS = 0;
		
		for (i = 4'H3; i <= 4'HA; i = i + 4'H1) begin
			#10	addr = i;
				dataReg = i;
				dataBusOE = 1;
			
			//#5	notCS = 0;
			#5	notWE = 0;
			#5	notWE = 1;
			//#5	notCS = 1;
				
			#5	dataBusOE = 0;
		end
		
		for(i = 4'H0; i <= 4'HF; i = i + 4'H1) begin
			#10	addr = i;
			
			//#5	notCS = 0;
			#5	notOE = 0;
				
			//#5	notCS = 1;
			#5	notOE = 1;
		end
		
		#30 $finish;
	end
	
	sram #(.DATA_WIDTH(4), .ADDR_WIDTH(4), .MEMFILE("mem.lst")) ram(addr, dataBus, notOE, notWE, notCS);

endmodule
