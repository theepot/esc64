`include "globals.v"
`include "gp_register.v"

//WAVEDUMPFILE macro should be defined

`define TEST_DELAY (500)
`define TEST_CLOCK (800)

module gp_register_test();
	wire [15:0] data_bus;
	reg [15:0] data;
	reg clock, reg0notLoad, reg0notOE, reg1notLoad, reg1notOE;
	reg oe;
	
	assign data_bus = oe ? data : 16'bzzzzzzzzzzzzzzzz;
	
	initial begin
		$dumpfile(`WAVEDUMPFILE);
		$dumpvars(0);
	
		clock = 0;
		reg0notOE = 1;
		reg0notLoad = 1;
		reg1notOE = 1;
		reg1notLoad = 1;
		data = 0;
		oe = 0;
		
		@ (posedge clock)
		#200

		if(data_bus !== 16'bzzzzzzzzzzzzzzzz) begin
			$display("ERROR:register. Got %X on outputs. Expected 0xZZZZ.", data_bus);
		end
		
		@ (posedge clock)
		#200
		reg0notOE = 0;
		
		# `TEST_DELAY if(data_bus !== 16'bxxxxxxxxxxxxxxxx) begin
			$display("ERROR:register. Got %X on outputs. Expected 0xxxxx.", data_bus);
		end
		
		@ (posedge clock)
		#200
		reg0notOE = 1;
		reg1notOE = 0;
		# `TEST_DELAY if(data_bus !== 16'bxxxxxxxxxxxxxxxx) begin
			$display("ERROR:register. Got %X on outputs. Expected 0xxxxx.", data_bus);
		end
		
		@ (posedge clock)
		#200
		reg1notOE = 1;
		oe = 1;
		data = 16'HBEEF;
		reg0notLoad = 0;
		
		@ (posedge clock)
		#200
		reg0notLoad = 1;
		reg0notOE = 0;
		oe = 0;
		reg1notLoad = 0;
		
		# `TEST_DELAY if(data_bus !== 16'HBEEF) begin
			$display("ERROR:register. Got %X on outputs. Expected 0xBEEF.", data_bus);
		end
		
		@ (posedge clock)
		#200
		reg0notOE = 1;
		reg1notLoad = 1;
		reg1notOE = 0;
		
		# `TEST_DELAY if(data_bus !== 16'HBEEF) begin
			$display("ERROR:register. Got %X on outputs. Expected 0xBEEF.", data_bus);
		end
		
		
		#10 $finish;
	end
	
	always begin
		# `TEST_CLOCK clock = ~clock;
	end

	gp_register register0(clock, reg0notLoad, reg0notOE, data_bus, data_bus);
	gp_register register1(clock, reg1notLoad, reg1notOE, data_bus, data_bus);

endmodule
