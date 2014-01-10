`include "globals.v"
`include "pc.v"

//WAVEDUMPFILE macro should be defined
`define CLOCK_HALFPERIOD 1000
`define SOME_DELAY 1000

module pc_test();
	wire [15:0] data_bus;
	reg [15:0] data;
	reg data_oe;
	
	reg clk;
	reg notReset;
	reg notOE;
	reg notLoad;
	reg inc;
	
	assign data_bus = data_oe ? data : 16'bZ;
	
	program_counter pc(clk, notReset, notLoad, notOE, inc, data_bus, data_bus);
	
	task check;
	input location;
	integer location;
	input [15:0] desired_data_bus;

	begin
		if(desired_data_bus !== data_bus) begin
			$display("ERROR @%d: data_bus is %B. Expected %B", location, data_bus, desired_data_bus);
		end
	end
	endtask

	integer i;
	initial begin
		$dumpfile(`WAVEDUMPFILE);
		$dumpvars(0);
		
		data_oe = 0;
		clk = 0;
		notReset = 1;
		notOE = 1;
		notLoad = 1;
		inc = 0;
		
		notReset = 0;
		#`SOME_DELAY;
		clk = 1;
		#`CLOCK_HALFPERIOD;
		clk = 0;
		#`CLOCK_HALFPERIOD;
		notReset = 1;
		
		#`SOME_DELAY;
		check(0, 16'hZ);
		
		clk = 1;
		#`SOME_DELAY;
		check(1, 16'hZ);
		clk = 0;
		#`SOME_DELAY;
		check(2, 16'hZ);
		
		notOE = 0;
		#`SOME_DELAY;
		check(3, 16'h0);
		
		for(i = 0; i < 32769; i = i + 1) begin
			inc = 1;
			#`SOME_DELAY;
			clk = 1;
			#`CLOCK_HALFPERIOD;
			check(10000+i, (i + 1) << 1);
			clk = 0;
			#`CLOCK_HALFPERIOD;
			check(50000+i, (i + 1) << 1);
		end
		
		notOE = 1;
		#`SOME_DELAY;
		check(4, 16'hZ);
		
		data_oe = 1;
		data = 16'hFBCF;
		#`SOME_DELAY;
		check(5, 16'hFBCF);
		
		clk = 1;
		#`SOME_DELAY;
		check(6, 16'hFBCF);
		clk = 0;
		#`SOME_DELAY;
		check(7, 16'hFBCF);
		
		data_oe = 0;
		notOE = 0;
		#`SOME_DELAY;
		check(8, 16'h0004);
		
		notOE = 1;
		data_oe = 1;
		notLoad = 0;
		#`SOME_DELAY;
		
		clk = 1;
		#`SOME_DELAY;
		clk = 0;
		#`SOME_DELAY;
		
		notOE = 0;
		data_oe = 0;
		notLoad = 1;
		
		check(9, 16'hFBCF);
		
		notReset = 0;
		#`SOME_DELAY;
		clk = 1;
		#`CLOCK_HALFPERIOD;
		clk = 0;
		#`CLOCK_HALFPERIOD;
		notReset = 1;
		
		check(10, 16'h0);
		
		$finish;
	end
	
	

endmodule
