`ifndef _ALU_INCLUDED_
`define _ALU_INCLUDED_

`include "../alu/alu_common.v"

module alu(a, b, yout, f, csel, ucin, fcin, cout, zout, notALUOE, notShiftOE);
	input   [15:0] a;
	input   [15:0] b;
	output  [15:0] yout;
	input   [4:0]  f;
	input   csel, ucin, fcin, notALUOE, notShiftOE;
	output	cout, zout;
	
	wire	[15:0] a;
	wire	[15:0] b;
	wire	[15:0] yout;
	reg		[15:0] y181;
	reg		[15:0] yshift;
	wire	[4:0]  f;
	wire	csel, ucin, fcin, notALUOE, notShiftOE;
	reg		cout;
	wire	zout;
	wire	cin;
	
	assign cin = csel ? fcin : ucin;
	assign yout = ~notALUOE ? y181 : 16'bzzzzzzzzzzzzzzzz;
	assign yout = ~notShiftOE ? yshift : 16'bzzzzzzzzzzzzzzzz;
	assign zout = yout === 16'H0000 ? 1'b1 : 1'b0;
	
	initial begin
		cout = ~(0);
	end
	
	always @ ( * )
	begin
		/*if(f[0] === 1'b1) begin
			if(!notShiftOE) begin
				cout = a >> 15;
			end
			yshift = a << 1;
		end
		else begin
			if(!notShiftOE) begin
				cout = a & 1;
			end
			yshift = a >> 1;
		end*/
		if(!notShiftOE) begin
			case(f)
				`ALU_F_SHIFT_RIGHT: begin
					cout = a & 1;
					yshift = a >> 1;
				end
				`ALU_F_SHIFT_LEFT: begin
					cout = a >> 15;
					yshift = a << 1;
				end
				default: $display("Warning in ALU: Illegal shift instruction %X", f);
			endcase
		end
		if(!notALUOE) begin
			case(f)
				`ALU_F_A: begin
					y181 = a + cin;
					if(notShiftOE) begin
						cout = y181 < a ? 1'b1 : 1'b0;
					end
				end
				`ALU_F_B:y181 = b;
				`ALU_F_SUB:begin
					y181 = a - b - 1 + cin;
					if(notShiftOE) begin
						cout = (a - 1 + cin) < b ? 1'b0 : 1'b1;
					end
				end
				`ALU_F_ADD:begin
					y181 = a + b + cin;
					if(notShiftOE) begin
						cout = y181 < a || y181 < b ? 1'b1 : 1'b0;
					end
				end
				`ALU_F_NOT:y181 = ~a;
				`ALU_F_XOR:y181 = a ^ b;
				`ALU_F_AND:y181 = a & b;
				`ALU_F_OR:y181 = a | b;
				default:$display("Warning in ALU. Unknown code at f");
			endcase
		end
	end
endmodule

`endif

