`include "rom.v"
module rom_test();
	reg [3:0] address;
	wire [3:0] data;
	integer i;
	
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0, address, data);
		

		for (i = 4'H0; i <= 4'HF; i = i + 4'H1) begin
			# 10 address = i;
		end
		
		
		
		#10 $finish;
	end
	
	rom #(.MEMFILE("rom.lst"), .DATA_WIDTH(4), .ADDR_WIDTH(4)) mem(address, data);

endmodule
