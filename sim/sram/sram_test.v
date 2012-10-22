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
			#100	addr = i;
					dataReg = i;
					dataBusOE = 1;
			
			#100	notWE = 0;
			#100	notWE = 1;
				
			#100	dataBusOE = 0;
		end
		
		#100	addr = 4'H0;
		
		#100	notOE = 0;
		#100	if(dataBus !== 4'HF) begin
			$display("ERROR: sram: addr=%X; data=%X; (data should be %X)", addr, dataBus, 4'HF);
		end
		#100	notOE = 1;
		
		#100	addr = 4'H1;
		
		#100	notOE = 0;
		#100	if(dataBus !== 4'HE) begin
			$display("ERROR: sram: addr=%X; data=%X; (data should be %X)", addr, dataBus, 4'HE);
		end
		#100	notOE = 1;
		
		for(i = 4'H3; i <= 4'HA; i = i + 4'H1) begin
			#100	addr = i;
			
			#100	notOE = 0;
			
			#100	if(dataBus !== i) begin
				$display("ERROR: sram: addr=%X; data=%X; (data should be %X)", addr, dataBus, i);
			end
			
			#100	notOE = 1;
		end
		
		#100	$finish;
	end
	
	sram #(.DATA_WIDTH(4), .ADDR_WIDTH(4), .MEMFILE("mem.lst")) ram(addr, dataBus, notOE, notWE, notCS);

endmodule
