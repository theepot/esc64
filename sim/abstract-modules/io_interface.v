`ifndef _IO_INTERFACE_INCLUDED_
`define _IO_INTERFACE_INCLUDED_

module anti_glitch_reg(clock, notReset, in, out, do_invert);
	input clock, notReset, in, do_invert;
	output out;
	wire clock, notReset, in, do_invert;
	reg out;
	
	always @ (negedge clock) begin
		if(notReset) begin
			out = do_invert ? ~in : in;
		end
	end
	
	always @ (negedge notReset) begin
		out = do_invert;
	end
endmodule

module io_interface(clock, notReset, a_bus, y_bus, rd, wr, select_dev, idle_n, word, dir_out, address_ld_n, data_ld_n, out_address, inout_data, out_rd_n, out_wr_n, out_csl_n, out_csh_n, out_select_dev);
	input clock, notReset, rd, wr, select_dev, idle_n, word, dir_out, address_ld_n, data_ld_n;
	inout [15:0] a_bus;
	inout [15:0] y_bus;
	output out_rd_n, out_wr_n, out_csl_n, out_csh_n, out_select_dev;
	inout [14:0] out_address;
	inout [15:0] inout_data;
	
	wire clock, notReset, rd, wr, select_dev, out_rd_n, out_wr_n, out_csl_n, out_csh_n, out_select_dev, idle_n, word, dir_out, address_ld_n, data_ld_n;
	wire [15:0] a_bus;
	wire [15:0] y_bus;
	wire [14:0] out_address;
	wire [15:0] inout_data;
	
	reg [15:0] data_reg;
	reg [15:0] address_reg;
	always @ (posedge clock) begin
		if(~data_ld_n) begin
			data_reg = y_bus;
		end
		if(~address_ld_n) begin
			address_reg = a_bus;
		end
	end
	
	anti_glitch_reg r1(clock, notReset, rd, out_rd_n, 1);
	anti_glitch_reg r2(clock, notReset, wr, out_wr_n, 1);
	anti_glitch_reg r3(clock, notReset, select_dev, out_select_dev, 0);
	
	wire idle_clean, word_clean, dir_out_clean;
	anti_glitch_reg r4(clock, notReset, idle_n, idle_clean, 1);
	anti_glitch_reg r5(clock, notReset, word, word_clean, 0);
	anti_glitch_reg r6(clock, notReset, dir_out, dir_out_clean, 0);
	
	wire address0 = address_reg[0];
	assign out_address = idle_clean ? 16'bZ : address_reg[15:1];
	assign inout_data = idle_clean || !dir_out_clean ? 16'bZ : ({word_clean ? data_reg[15:8] : (address0 ? data_reg[7:0] : 8'bZ), !address0 ? data_reg[7:0] : 8'bZ});
	assign y_bus = idle_clean || dir_out_clean ? 16'bZ : ({!word_clean ? 8'b0 : inout_data[15:8], address0 ? inout_data[15:8] : inout_data[7:0]});
	
	assign out_csl_n = ~(~idle_clean && ~address0);
	assign out_csh_n = ~(~idle_clean && (address0 || word_clean));
	
	always @ (address0, word_clean, idle_clean) begin
		if(address0 && word_clean && !idle_clean) begin
			$display("WARNING: trying to access unaligned word");
		end
	end
endmodule

`endif
