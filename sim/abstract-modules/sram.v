`ifndef _SRAM_INCLUDED_
`define _SRAM_INCLUDED_

module sram(addr, data, notOE, notWE, notCS);
	parameter ADDR_WIDTH = 16;
	parameter DATA_WIDTH = 16;
	parameter MEMFILE = "";
	
	input [ADDR_WIDTH-1:0] addr;
	wire [ADDR_WIDTH-1:0] addr;
	
	inout [DATA_WIDTH-1:0] data;
	wire [DATA_WIDTH-1:0] data;
	
	input notOE, notWE, notCS;
	wire notOE, notWE, notCS;
	
	reg [DATA_WIDTH-1:0] mem[0:((1<<ADDR_WIDTH)-1)];

	assign data = (!notCS && !notOE) ? mem[addr] : 16'bzzzzzzzzzzzzzzzz;
	
	function maybe_high;
	input x;
	begin
		maybe_high = (^x === 1'bx || x === 1'b1);
	end
	endfunction
	
	function maybe_low;
	input x;
	begin
		maybe_low = (^x === 1'bx || x === 1'b0);
	end
	endfunction
	
	task test_state;
	begin
		if((^notCS === 1'bx && (maybe_low(notWE) || maybe_low(notOE)))
		|| (notCS === 1'b0 && ((^notWE === 1'bx || ^notOE === 1'bx) || (notWE === 1'b0 && notOE === 1'b0))))
		begin
			$display("warning: in sram: notCS=%X; notWE=%X; notOE=%X", notCS, notWE, notOE);
		end
	end
	endtask
	
	initial begin
		if(MEMFILE != "") begin
			$readmemb(MEMFILE, mem);
		end
		else begin
			$display("warning: sram: no memory file defined");
		end
		
	end

	always @ (posedge notWE) begin
		if(!notCS) begin
			mem[addr] = data;
		end
	end

endmodule

`endif
