`ifndef _REGSEL_INCLUDED_
`define _REGSEL_INCLUDED_

module regSel(oe, load, oeSourceSel, loadSourceSel, useqRegSelOE, useqRegSelLoad, op0, op1, op2, regNotOEs, regNotLoads);
	input oe, load;
	input [1:0] oeSourceSel;
	input loadSourceSel;
	input [2:0] useqRegSelOE, useqRegSelLoad, op0, op1, op2;
	output [7:0] regNotOEs, regNotLoads;
	
	wire oe, load;
	wire [1:0] oeSourceSel;
	wire loadSourceSel;
	wire [2:0] useqRegSelOE, useqRegSelLoad, op0, op1, op2;
	reg [7:0] regNotOEs, regNotLoads;
	
	always @( * ) begin
		if(oe) begin
			case(oeSourceSel)
				2'b00: regNotOEs = ~(1 << (useqRegSelOE));
				2'b01: regNotOEs = ~(1 << (op0));
				2'b10: regNotOEs = ~(1 << (op1));
				2'b11: regNotOEs = ~(1 << (op2));
			endcase
		end
		else begin
			regNotOEs = 8'HFF;
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

