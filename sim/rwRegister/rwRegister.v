`ifndef _WRREG_INCLUDED_
`define _WRREG_INCLUDED_

module rwRegister(clock, notReset, wr, rd, notWR, notRD);
	input clock, notReset, wr, rd;
	wire clock, notReset, wr, rd;
	
	output notWR, notRD;
	
	wire notWR = notReset ? notWR_int : 1'b1;
	wire notRD = notReset ? notRD_int : 1'b1;
	
	reg notWR_int, notRD_int;
	
	always @ (negedge clock) begin
		if(notReset) begin
			notWR_int = ~wr;
			notRD_int = ~rd;
		end
	end
	
	always @ (negedge notReset) begin
		notWR_int = 1'b1;
		notRD_int = 1'b1;
	end

endmodule


`endif
