`ifndef _IO_INTERFACE_INCLUDED_
`define _IO_INTERFACE_INCLUDED_

module anti_glitch_reg(clock, notReset, in, out_n);
	input clock, notReset, in;
	output out_n;
	wire clock, notReset, in;
	reg out_n;
	
	always @ (negedge clock) begin
		if(notReset) begin
			out_n = ~in;
		end
	end
	
	always @ (negedge notReset) begin
		out_n = 1'b1;
	end
endmodule

module io_interface(clock, notReset, a_bus, y_bus, rd, wr, data_dir_in, data_dir_out, address_ld_n, data_ld_n, out_address, inout_data, out_rd_n, out_wr_n);
	input clock, notReset, rd, wr, data_dir_in, data_dir_out, address_ld_n, data_ld_n;
	inout [15:0] a_bus;
	inout [15:0] y_bus;
	output out_rd_n, out_wr_n;
	inout [15:0] out_address;
	inout [15:0] inout_data;
	
	wire clock, notReset, rd, wr, out_rd_n, out_wr_n, data_dir_in, data_dir_out, address_ld_n, data_ld_n;
	wire [15:0] a_bus;
	wire [15:0] y_bus;
	wire [15:0] out_address;
	wire [15:0] inout_data;
	
	anti_glitch_reg rd_anti_glitch(clock, notReset, rd, out_rd_n);
	anti_glitch_reg wr_anti_glitch(clock, notReset, wr, out_wr_n);
	
	wire data_dir_out_clean_n;
	anti_glitch_reg data_dir_out_anti_glitch(clock, notReset, data_dir_out, data_dir_out_clean_n);
	
	wire address_oe;
	assign address_oe = data_dir_in | data_dir_out;
	wire address_oe_clean_n;
	anti_glitch_reg address_oe_n(clock, notReset, address_oe, address_oe_clean_n);
	
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
	
	assign out_address = address_oe_clean_n ? 16'bZ : address_reg;
	
	assign inout_data = data_dir_out_clean_n ? 16'bZ : data_reg;
	
	assign y_bus = data_dir_in ? inout_data : 16'bZ;

endmodule

`endif
