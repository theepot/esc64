`include "pc.v"

module pc_test();
	wire [15:0] data_bus;
	reg [15:0] data;
	reg dataE;
	
	reg clk;
	reg notClr;
	reg notWrite;
	reg read;
	reg inc;
	  	
	assign data_bus = dataE ? data : 16'bzzzzzzzzzzzzzzzz;
	
	
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0, data_bus, data, dataE, clk, notClr, notWrite, read, inc);
	
		clk = 0;
		notClr = 1;
		notWrite = 1;
		read = 0;
		inc = 0;
		
		data = 0;
		dataE = 0;
		
		#100	data = 16'HDEAD;
		#100	dataE = 1;
				notWrite = 0;
		#100	notWrite = 1;
				dataE = 0;
		#100	read = 1;
		
		#100	if(data_bus !== 16'HDEAD) begin
			$display("ERROR: pc: data_bus=%X (should be DEAD)", data_bus);
		end
		
		#100	read = 0;
		
		#100	inc = 1;
		#20		inc = 0;
		
		#100	read = 1;
		
		#100	if(data_bus !== 16'HDEAE) begin
			$display("ERROR: pc: data_bus=%X (should be DEAE)", data_bus);
		end
		
		#100	read = 0;
		
		#100	$finish;
	end
	
	always begin
		#20 clk = ~clk;
	end
	
	program_counter #(.DATA_WIDTH(16)) pc(clk, notClr, notWrite, read, inc, data_bus, data_bus);

endmodule
