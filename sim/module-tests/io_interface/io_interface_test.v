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
	
	wire [14:0] address_external_bus;
	
	reg clock, notReset, rd, wr, dir_out, word, idle_n, select_dev, address_ld_n, data_ld_n;
	wire out_rd_n, out_wr_n, out_csh_n, out_csl_n, out_select_dev;
	
	assign data_internal_bus = data_internal_oe ? data_internal : 16'bZ;
	assign data_external_bus = data_external_oe ? data_external : 16'bZ;
	assign address_internal_bus = address_internal_oe ? address_internal : 16'bZ;
	
	io_interface target(clock, notReset, address_internal_bus, data_internal_bus, rd, wr, select_dev, idle_n, word, dir_out, address_ld_n, data_ld_n, address_external_bus, data_external_bus, out_rd_n, out_wr_n, out_csl_n, out_csh_n, out_select_dev);
	
	task check;
	input location;
	integer location;
	input [15:0] desired_data_internal_bus;
	input [15:0] desired_data_external_bus;
	input [14:0] desired_address_external_bus;
	input desired_rd_n;
	input desired_wr_n;
	input desired_csh_n;
	input desired_csl_n;
	input desired_select_dev;
	
	begin
		if(desired_data_internal_bus !== 16'bX && desired_data_internal_bus !== data_internal_bus) begin
			$display("ERROR @%d: data_internal_bus is %B. Expected %B", location, data_internal_bus, desired_data_internal_bus);
		end
		if(desired_data_external_bus !== 16'bX && desired_data_external_bus !== data_external_bus) begin
			$display("ERROR @%d: data_external_bus is %B. Expected %B", location, data_external_bus, desired_data_external_bus);
		end
		if(desired_address_external_bus !== 15'bX && desired_address_external_bus !== address_external_bus) begin
			$display("ERROR @%d: address_external_bus is %B. Expected %B", location, address_external_bus, desired_address_external_bus);
		end
		if(desired_rd_n !== 1'bx && desired_rd_n !== out_rd_n) begin
			$display("ERROR @%d: out_rd_n is %X. Expected %X", location, out_rd_n, desired_rd_n);
		end
		if(desired_wr_n !== 1'bx && desired_wr_n !== out_wr_n) begin
			$display("ERROR @%d: out_wr_n is %X. Expected %X", location, out_wr_n, desired_wr_n);
		end
		if(desired_csh_n !== 1'bx && desired_csh_n !== out_csh_n) begin
			$display("ERROR @%d: out_csh_n is %X. Expected %X", location, out_csh_n, desired_csh_n);
		end
		if(desired_csl_n !== 1'bx && desired_csl_n !== out_csl_n) begin
			$display("ERROR @%d: out_csl_n is %X. Expected %X", location, out_csl_n, desired_csl_n);
		end
		if(desired_select_dev !== 1'bx && desired_select_dev !== out_select_dev) begin
			$display("ERROR @%d: out_select_dev is %X. Expected %X", location, out_select_dev, desired_select_dev);
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
		select_dev = 0;
		idle_n = 0;
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
		
		check(1, 16'bZ, 16'bZ, 16'bZ, 1, 1, 1, 1, 0);
		
		//read, write and select_dev proagate
		rd = 1;
		wr = 0;
		select_dev = 1;
		#`SOME_DELAY;
		
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(2, 16'bZ, 16'bZ, 16'bZ, 1, 1, 1, 1, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(3, 16'bZ, 16'bZ, 16'bZ, 0, 1, 1, 1, 1);
		
		rd = 0;
		wr = 1;
		select_dev = 0;
		
		#`SOME_DELAY;
		
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(4, 16'bZ, 16'bZ, 16'bZ, 0, 1, 1, 1, 1);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(5, 16'bZ, 16'bZ, 16'bZ, 1, 0, 1, 1, 0);
		
		#`SOME_DELAY;
		
		//word data out
		data_internal = 16'hABCD;
		address_internal = 16'h0120;
		data_internal_oe = 1;
		address_internal_oe = 1;
		idle_n = 1;
		dir_out = 1;
		word = 1;
		
		#`SOME_DELAY;
		
		check(6, 16'hABCD, 16'bZ, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(7, 16'hABCD, 16'bZ, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(8, 16'hABCD, 16'hABCD, 16'h0120 >> 1, 1'bx, 1'bx, 0, 0, 0);
		
		data_internal = 16'h4567;
		address_internal = 16'h89A2;
		#`SOME_DELAY;
		check(9, 16'h4567, 16'hABCD, 16'h0120 >> 1, 1'bx, 1'bx, 0, 0, 0);
		
		data_ld_n = 1;
		address_ld_n = 1;
		#`SOME_DELAY;
		check(10, 16'h4567, 16'hABCD, 16'h0120 >> 1, 1'bx, 1'bx, 0, 0, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(11, 16'h4567, 16'hABCD, 16'h0120 >> 1, 1'bx, 1'bx, 0, 0, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(12, 16'h4567, 16'hABCD, 16'h0120 >> 1, 1'bx, 1'bx, 0, 0, 0);
		
		idle_n = 0;
		data_ld_n = 0;
		address_ld_n = 0;
		#`SOME_DELAY;
		check(13, 16'h4567, 16'hABCD, 16'h0120 >> 1, 1'bx, 1'bx, 0, 0, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(14, 16'h4567, 16'h4567, $unsigned(16'h89A2) >> 1, 1'bx, 1'bx, 0, 0, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(15, 16'h4567, 16'bZ, 16'bZ, 1'bx, 1'bx, 1, 1, 0);


		//byte data out. alligned
		data_internal = 16'hXXCD;
		address_internal = 16'h0120;
		data_internal_oe = 1;
		address_internal_oe = 1;
		idle_n = 1;
		dir_out = 1;
		word = 0;
		
		#`SOME_DELAY;
		
		check(16, 16'hXXCD, 16'bZ, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(17, 16'hXXCD, 16'bZ, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(18, 16'hXXCD, 16'hZZCD, 16'h0120 >> 1, 1'bx, 1'bx, 1, 0, 0);
		
		data_internal = 16'hXX67;
		address_internal = 16'h89A2;
		#`SOME_DELAY;
		check(19, 16'hXX67, 16'hZZCD, 16'h0120 >> 1, 1'bx, 1'bx, 1, 0, 0);
		
		data_ld_n = 1;
		address_ld_n = 1;
		#`SOME_DELAY;
		check(20, 16'hXX67, 16'hZZCD, 16'h0120 >> 1, 1'bx, 1'bx, 1, 0, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(21, 16'hXX67, 16'hZZCD, 16'h0120 >> 1, 1'bx, 1'bx, 1, 0, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(22, 16'hXX67, 16'hZZCD, 16'h0120 >> 1, 1'bx, 1'bx, 1, 0, 0);
		
		idle_n = 0;
		data_ld_n = 0;
		address_ld_n = 0;
		#`SOME_DELAY;
		check(23, 16'hXX67, 16'hZZCD, 16'h0120 >> 1, 1'bx, 1'bx, 1, 0, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(24, 16'hXX67, 16'hZZ67, $unsigned(16'h89A2) >> 1, 1'bx, 1'bx, 1, 0, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(25, 16'hXX67, 16'bZ, 16'bZ, 1'bx, 1'bx, 1, 1, 0);

		//byte data out. unalligned
		data_internal = 16'hXXCD;
		address_internal = 16'h0121;
		data_internal_oe = 1;
		address_internal_oe = 1;
		idle_n = 1;
		dir_out = 1;
		word = 0;
		
		#`SOME_DELAY;
		
		check(26, 16'hXXCD, 16'bZ, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(27, 16'hXXCD, 16'bZ, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(28, 16'hXXCD, 16'hCDZZ, 16'h0121 >> 1, 1'bx, 1'bx, 0, 1, 0);
		
		data_internal = 16'hXX67;
		address_internal = 16'h89A3;
		#`SOME_DELAY;
		check(29, 16'hXX67, 16'hCDZZ, 16'h0121 >> 1, 1'bx, 1'bx, 0, 1, 0);
		
		data_ld_n = 1;
		address_ld_n = 1;
		#`SOME_DELAY;
		check(30, 16'hXX67, 16'hCDZZ, 16'h0121 >> 1, 1'bx, 1'bx, 0, 1, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(31, 16'hXX67, 16'hCDZZ, 16'h0121 >> 1, 1'bx, 1'bx, 0, 1, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(32, 16'hXX67, 16'hCDZZ, 16'h0121 >> 1, 1'bx, 1'bx, 0, 1, 0);
		
		idle_n = 0;
		data_ld_n = 0;
		address_ld_n = 0;
		#`SOME_DELAY;
		check(33, 16'hXX67, 16'hCDZZ, 16'h0121 >> 1, 1'bx, 1'bx, 0, 1, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(34, 16'hXX67, 16'h67ZZ, $unsigned(16'h89A3) >> 1, 1'bx, 1'bx, 0, 1, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(35, 16'hXX67, 16'bZ, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		
		
		//word in
		data_internal = 16'bX;
		data_internal_oe = 0;
		address_internal = 16'hF4D6;
		address_internal_oe = 1;
		data_external = 16'hEA53;
		data_external_oe = 1;
		dir_out = 0;
		word = 1;
		idle_n = 1;
		data_ld_n = 1;
		address_ld_n = 0;
		
		#`SOME_DELAY;
		
		check(36, 16'hZ, 16'hEA53, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(37, 16'hZ, 16'hEA53, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(38, 16'hEA53, 16'hEA53, $unsigned(16'hF4D6) >> 1, 1'bx, 1'bx, 0, 0, 0);
		
		data_external = 16'h0F8B;
		address_internal = 16'h9752;
		
		#`SOME_DELAY;
		check(39, 16'h0F8B, 16'h0F8B, $unsigned(16'hF4D6) >> 1, 1'bx, 1'bx, 0, 0, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(40, 16'h0F8B, 16'h0F8B, $unsigned(16'h9752) >> 1, 1'bx, 1'bx, 0, 0, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(41, 16'h0F8B, 16'h0F8B, $unsigned(16'h9752) >> 1, 1'bx, 1'bx, 0, 0, 0);
		
		idle_n = 0;
		#`SOME_DELAY;
		check(42, 16'h0F8B, 16'h0F8B, $unsigned(16'h9752) >> 1, 1'bx, 1'bx, 0, 0, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(43, 16'h0F8B, 16'h0F8B, $unsigned(16'h9752) >> 1, 1'bx, 1'bx, 0, 0, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(44, 16'hZ, 16'h0F8B, 16'hZ, 1'bx, 1'bx, 1, 1, 0);
		
		//alligned byte in
		data_internal = 16'bX;
		data_internal_oe = 0;
		address_internal = 16'hF4D6;
		address_internal_oe = 1;
		data_external = 16'hXX53;
		data_external_oe = 1;
		dir_out = 0;
		word = 0;
		idle_n = 1;
		data_ld_n = 1;
		address_ld_n = 0;
		
		#`SOME_DELAY;
		
		check(45, 16'hZ, 16'hXX53, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(46, 16'hZ, 16'hXX53, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(47, 16'h0053, 16'hXX53, $unsigned(16'hF4D6) >> 1, 1'bx, 1'bx, 1, 0, 0);
		
		data_external = 16'hXX8B;
		address_internal = 16'h9752;
		
		#`SOME_DELAY;
		check(48, 16'h008B, 16'hXX8B, $unsigned(16'hF4D6) >> 1, 1'bx, 1'bx, 1, 0, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(49, 16'h008B, 16'hXX8B, $unsigned(16'h9752) >> 1, 1'bx, 1'bx, 1, 0, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(50, 16'h008B, 16'hXX8B, $unsigned(16'h9752) >> 1, 1'bx, 1'bx, 1, 0, 0);
		
		idle_n = 0;
		#`SOME_DELAY;
		check(51, 16'h008B, 16'hXX8B, $unsigned(16'h9752) >> 1, 1'bx, 1'bx, 1, 0, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(52, 16'h008B, 16'hXX8B, $unsigned(16'h9752) >> 1, 1'bx, 1'bx, 1, 0, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(53, 16'hZ, 16'hXX8B, 16'hZ, 1'bx, 1'bx, 1, 1, 0);
		
		//unalligned byte in
		data_internal = 16'bX;
		data_internal_oe = 0;
		address_internal = 16'hF4D3;
		address_internal_oe = 1;
		data_external = 16'hEAXX;
		data_external_oe = 1;
		dir_out = 0;
		word = 0;
		idle_n = 1;
		data_ld_n = 1;
		address_ld_n = 0;
		
		#`SOME_DELAY;
		
		check(54, 16'hZ, 16'hEAXX, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(55, 16'hZ, 16'hEAXX, 16'bZ, 1'bx, 1'bx, 1, 1, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(56, 16'h00EA, 16'hEAXX, $unsigned(16'hF4D3) >> 1, 1'bx, 1'bx, 0, 1, 0);
		
		data_external = 16'h0FXX;
		address_internal = 16'h9751;
		
		#`SOME_DELAY;
		check(57, 16'h000F, 16'h0FXX, $unsigned(16'hF4D3) >> 1, 1'bx, 1'bx, 0, 1, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(58, 16'h000F, 16'h0FXX, $unsigned(16'h9751) >> 1, 1'bx, 1'bx, 0, 1, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(59, 16'h000F, 16'h0FXX, $unsigned(16'h9751) >> 1, 1'bx, 1'bx, 0, 1, 0);
		
		idle_n = 0;
		#`SOME_DELAY;
		check(60, 16'h000F, 16'h0FXX, $unsigned(16'h9751) >> 1, 1'bx, 1'bx, 0, 1, 0);
		#`CLOCK_HALFPERIOD clock = 1;
		#`SOME_DELAY;
		check(61, 16'h000F, 16'h0FXX, $unsigned(16'h9751) >> 1, 1'bx, 1'bx, 0, 1, 0);
		#`CLOCK_HALFPERIOD clock = 0;
		#`SOME_DELAY;
		check(62, 16'hZ, 16'h0FXX, 16'hZ, 1'bx, 1'bx, 1, 1, 0);

		
	end
	


endmodule
