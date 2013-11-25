`include "io_interface.v"

//WAVEDUMPFILE macro should be defined

`define CLOCK_HALFPERIOD 1000
`define SOME_DELAY 1000

module io_interface_test();
	reg [15:0] data_internal;
	reg	data_internal_oe;
	wire [15:0] data_internal_bus;

	reg [15:0] data_external;
	reg data_external_oe;
	wire [15:0] data_external_bus;

	reg [15:0] address_internal;
	reg address_internal_oe;
	wire [15:0] address_internal_bus;
	
	wire [15:0] address_external_bus;
	
	reg clock, notReset, rd, wr, data_dir_in, data_dir_out, address_ld_n, data_ld_n;
	wire out_rd_n, out_wr_n;
	
	assign data_internal_bus = data_internal_oe ? data_internal : 16'bZ;
	assign data_external_bus = data_external_oe ? data_external : 16'bZ;
	assign address_internal_bus = address_internal_oe ? address_internal : 16'bZ;
	
	io_interface target(clock, notReset, address_internal_bus, data_internal_bus, rd, wr, data_dir_in, data_dir_out, address_ld_n, data_ld_n, address_external_bus, data_external_bus, out_rd_n, out_wr_n);
	
	task check;
	input location;
	integer location;
	input [15:0] desired_data_internal_bus;
	input [15:0] desired_data_external_bus;
	input [15:0] desired_address_external_bus;
	input desired_rd_n;
	input desired_wr_n;
	begin
		if(desired_data_internal_bus !== 16'bX && desired_data_internal_bus !== data_internal_bus) begin
			$display("ERROR @%d: data_internal_bus is %X. Expected %X", location, data_internal_bus, desired_data_internal_bus);
		end
		if(desired_data_external_bus !== 16'bX && desired_data_external_bus !== data_external_bus) begin
			$display("ERROR @%d: data_external_bus is %X. Expected %X", location, data_external_bus, desired_data_external_bus);
		end
		if(desired_address_external_bus !== 16'bX && desired_address_external_bus !== address_external_bus) begin
			$display("ERROR @%d: address_external_bus is %X. Expected %X", location, address_external_bus, desired_address_external_bus);
		end
		if(desired_rd_n !== 1'bx && desired_rd_n !== out_rd_n) begin
			$display("ERROR @%d: out_rd_n is %X. Expected %X", location, out_rd_n, desired_rd_n);
		end
		if(desired_wr_n !== 1'bx && desired_wr_n !== out_wr_n) begin
			$display("ERROR @%d: out_wr_n is %X. Expected %X", location, out_wr_n, desired_wr_n);
		end
	end
	endtask
	
	initial begin
		$dumpfile(`WAVEDUMPFILE);
		$dumpvars(0);
		
		clock = 0;
		notReset = 1;
		
		data_internal = 16'h0000;
		data_internal_oe = 0;
		
		data_external = 16'h0000;
		data_external_oe = 0;
		
		address_internal = 16'h0000;
		address_internal_oe = 0;
		
		rd = 1;
		wr = 1;
		data_dir_in = 0;
		data_dir_out = 0;
		address_ld_n = 0;
		data_ld_n = 0;
		
		//reset
		
		notReset = 0;
		
		#`SOME_DELAY;
		
		#`CLOCK_HALFPERIOD clock = 1;
		#`CLOCK_HALFPERIOD clock = 0;
		
		#`SOME_DELAY;
		
		notReset = 1;
		
		#`SOME_DELAY;
		
		check(1, 16'bZ, 16'bZ, 16'bZ, 1, 1);
		
		//read and write proagate
		rd = 1;
		wr = 0;
		#`SOME_DELAY;
		
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(2, 16'bZ, 16'bZ, 16'bZ, 1, 1);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(3, 16'bZ, 16'bZ, 16'bZ, 0, 1);
		
		rd = 0;
		wr = 1;
		
		#`SOME_DELAY;
		
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(4, 16'bZ, 16'bZ, 16'bZ, 0, 1);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(5, 16'bZ, 16'bZ, 16'bZ, 1, 0);
		
		#`SOME_DELAY;
		
		//data out, address out
		data_internal = 16'hABCD;
		address_internal = 16'h0123;
		data_internal_oe = 1;
		address_internal_oe = 1;
		data_dir_in = 0;
		data_dir_out = 1;
		
		#`SOME_DELAY;
		
		check(6, 16'hABCD, 16'bZ, 16'bZ, 1'bx, 1'bx);
		
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(7, 16'hABCD, 16'bZ, 16'bZ, 1'bx, 1'bx);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(8, 16'hABCD, 16'hABCD, 16'h0123, 1'bx, 1'bx);
		
		data_internal = 16'h4567;
		address_internal = 16'h89AB;
		#`SOME_DELAY;
		check(9, 16'h4567, 16'hABCD, 16'h0123, 1'bx, 1'bx);
		
		data_ld_n = 1;
		address_ld_n = 1;
		#`SOME_DELAY;
		check(10, 16'h4567, 16'hABCD, 16'h0123, 1'bx, 1'bx);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(10, 16'h4567, 16'hABCD, 16'h0123, 1'bx, 1'bx);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(10, 16'h4567, 16'hABCD, 16'h0123, 1'bx, 1'bx);

		
		data_dir_out = 0;
		data_ld_n = 0;
		address_ld_n = 0;
		#`SOME_DELAY;
		check(10, 16'h4567, 16'hABCD, 16'h0123, 1'bx, 1'bx);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(11, 16'h4567, 16'h4567, 16'h89AB, 1'bx, 1'bx);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(12, 16'h4567, 16'bZ, 16'bZ, 1'bx, 1'bx);

		
		//data in, address out
		data_internal = 16'bX;
		data_internal_oe = 0;
		address_internal = 16'h2934;
		address_internal_oe = 1;
		data_external = 16'h12BA;
		data_external_oe = 1;
		data_dir_in = 1;
		data_dir_out = 0;
		
		#`SOME_DELAY;
		
		check(13, 16'h12BA, 16'h12BA, 16'bZ, 1'bx, 1'bx);
		
		#`CLOCK_HALFPERIOD clock = 1;
		
		#`SOME_DELAY;
		check(14, 16'h12BA, 16'h12BA, 16'bZ, 1'bx, 1'bx);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(15, 16'h12BA, 16'h12BA, 16'h2934, 1'bx, 1'bx);
		
		data_external_oe = 0;
		#`SOME_DELAY;
		check(16, 16'bZ, 16'bZ, 16'h2934, 1'bx, 1'bx);

		data_dir_in = 0;
		data_external_oe = 1;
		address_internal = 16'h9233;
		
		#`SOME_DELAY;
		check(17, 16'bZ, 16'h12BA, 16'h2934, 1'bx, 1'bx);
		
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(18, 16'bZ, 16'h12BA, 16'h9233, 1'bx, 1'bx);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(19, 16'bZ, 16'h12BA, 16'bZ, 1'bx, 1'bx);
		
		
	
	end
	


endmodule
