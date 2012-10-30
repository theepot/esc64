`ifdef PC_STRUCT
`include "pc_s.v"
`define WAVENAME "pc_s.vcd"
`else
`include "pc.v"
`define WAVENAME "pc.vcd"
`endif

`include "../globals.v"

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
		$dumpfile(`WAVENAME);
		$dumpvars(0);
	
		clk = 0;
		notClr = 1;
		notWrite = 1;
		read = 1;
		inc = 0;
		
		data = 0;
		dataE = 0;
		
		#100	data = 16'HFFFF;
		#100	dataE = 1;
				notWrite = 0;
		#100	notWrite = 1;
				dataE = 0;
		#100	read = 0;
		
		#100	if(data_bus !== 16'HFFFF) begin
			$display("ERROR: pc: data_bus=%X (should be DEAD)", data_bus);
		end
		
		#100	read = 1;
		
		#100	inc = 1;
		#20		inc = 0;
		
		#100	read = 0;
		
		#100	if(data_bus !== 16'H0000) begin
			$display("ERROR: pc: data_bus=%X (should be 0000)", data_bus);
		end
		
		#100	read = 1;
		
		#100	$finish;
	end
	
	always begin
		#20 clk = ~clk;
	end
	
	program_counter pc(clk, notClr, notWrite, read, inc, data_bus, data_bus);

endmodule
