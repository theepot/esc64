`ifndef _REGSEL_INCLUDED_
`define _REGSEL_INCLUDED_

module regSel(oe, load, oeSourceSel, loadSourceSel, useqRegSelOe, useqRegSelLoad, op0, op1, op2, regOes, regNotLoads);
	input oe, load;
	input [1:0] oeSourceSel;
	input loadSourceSel;
	input [2:0] useqRegSelOe, useqRegSelLoad, op0, op1, op2;
	output [7:0] regOes, regNotLoads;
	
	wire oe, load;
	wire [1:0] oeSourceSel;
	wire loadSourceSel;
	wire [2:0] useqRegSelOe, useqRegSelLoad, op0, op1, op2;
	reg [7:0] regOes, regNotLoads;
	
	always @( * ) begin
		if(oe) begin
			case(oeSourceSel)
				2'b00: regOes = 1 << (useqRegSelOe);
				2'b01: regOes = 1 << (op0);
				2'b10: regOes = 1 << (op1);
				2'b11: regOes = 1 << (op2);
			endcase
		end
		else begin
			regOes = 8'H00;
		end
		
		if(load) begin
			case(loadSourceSel)
				1'b0: regNotLoads = ~(1 << (useqRegSelLoad));
				1'b1: regNotLoads = ~(1 << (op0));
			endcase
		end
		else begin
			regNotLoads = 8'HFF;
		end
	end

endmodule

`endif

