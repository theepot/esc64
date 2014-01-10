`ifndef _VIRTUAL_IO_INCLUDED_
`define _VIRTUAL_IO_INCLUDED_

module virtual_io(addr, data, rd_n, wr_n, csh_n, csl_n, select_dev);
	parameter ADDR_WIDTH = 15;
	
	input [ADDR_WIDTH-1:0] addr;
	inout [15:0] data;
	input rd_n, wr_n, csh_n, csl_n, select_dev;
	
	
	reg [15:0] data_reg;
	assign #1 data = {(!csh_n && !rd_n) ? data_reg[15:8] : 8'hZ, (!csl_n && !rd_n) ? data_reg[7:0] : 8'hZ};
	
	always @ (negedge rd_n) begin
		if(wr_n !== 1'b1) begin
			$display("virtual_io: WARNING: wr_n is not inactive at negedge of rd_n");
			$finish;
		end
		if(^csh_n === 1'bx || ^csl_n === 1'bx || ^select_dev === 1'bx) begin
			$display("virtual_io: ERROR: during read, one of the control signals is unknown or high-impedance");
			$finish;
		end
		if(^addr === 1'bx) begin
			$display("virtual_io: ERROR: during read, one or more of the bits of the address is unknown or high-impedance");
			$finish;
		end
		if(csh_n == 1'b1 && csl_n == 1'b1) begin
			$display("virtual_io: ERROR: during read, both csh and csl are inactive");
			$finish;
		end
		
		data_reg = $virtio_read(addr, ~csh_n, ~csl_n, select_dev);
	end
	
	always @ (negedge wr_n) begin
		if(rd_n !== 1'b1) begin
			$display("virtual_io: WARNING: rd_n is not inactive at negedge of wr_n");
		end
		if(^csh_n === 1'bx || ^csl_n === 1'bx || ^select_dev === 1'bx) begin
			$display("virtual_io: ERROR: during write, one of the control signals is unknown or high-impedance");
		end
		if(^addr === 1'bx) begin
			$display("virtual_io: ERROR: during write, one or more of the bits of the address is unknown or high-impedance");
		end
		if(csh_n == 1'b1 && csl_n == 1'b1) begin
			$display("virtual_io: ERROR: during write, both csh and csl are inactive");
		end
		
		$virtio_write(addr, data, ~csh_n, ~csl_n, select_dev);
	end
	

endmodule

`endif
