`include "pc.v"

module pc_test();
	wire [15:0] data_bus;
	reg [15:0] data;
	reg oe;
	
	reg clk;
	reg notClr;
	reg notWrite;
	reg read;
	reg inc;
	  	
	assign data_bus = oe ? data : 16'bzzzzzzzzzzzzzzzz;
	
	
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0, data_bus, data, oe, clk, notClr, notWrite, read, inc);
	
		clk = 0;
		notClr = 1;
		notWrite = 1;
		read = 0;
		inc = 0;
		
		data = 0;
		oe = 0;
		
		#20	data = 16'HDEAD;
		#10	oe = 1;
		notWrite = 0;
		#10	notWrite = 1;
		oe = 0;
		
		#10	inc = 1;
		#10	inc = 0;
		
		#20	read = 1;
		#10 read = 0;
		
		#30	$finish;
	end
	
	always begin
		#5 clk = ~clk;
	end
	
	program_counter #(.DATA_WIDTH(16)) pc(clk, notClr, notWrite, read, inc, data_bus, data_bus);

endmodule
