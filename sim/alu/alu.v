`ifndef _ALU_INCLUDED_
`define _ALU_INCLUDED_

`define ALU_F_A				5'b00000
`define ALU_F_B				5'b10101
`define ALU_F_SUB			5'b01100
`define ALU_F_ADD			5'b10010
`define ALU_F_NOT			5'b00001
`define ALU_F_XOR			5'b01111
`define ALU_F_AND			5'b10111
`define ALU_F_OR			5'b11101

`define ALU_F_SHIFT_LEFT	5'b00001
`define ALU_F_SHIFT_RIGHT	5'b00000
`define ALU_FSEL_74181		0
`define ALU_FSEL_SHIFT		1
`define ALU_CSEL_UCIN		0
`define ALU_CSEL_FCIN		1

module alu(a, b, yout, f, fsel, csel, ucin, fcin, cout, zout, yoe);
	input   [15:0] a;
	input   [15:0] b;
	output  [15:0] yout;
	input   [4:0]  f;
	input   fsel, csel, ucin, fcin, yoe;
	output	cout, zout;
	
	wire	[15:0] a;
	wire	[15:0] b;
	wire	[15:0] yout;
	reg		[15:0] y;
	wire	[4:0]  f;
	wire	fsel, csel, ucin, fcin;
	reg		cout, zout;
	wire	cin;
	
	assign cin = csel ? fcin : ucin;
	assign yout = yoe ? y : 16'bzzzzzzzzzzzzzzzz;
	
	initial begin
		cout = 0;
	end
	
	always @ ( * )
	begin
		if(fsel) begin
			if(f[0] === 1'b1) begin
				y = a << 1;
			end
			else begin
				y = a >> 1;
			end
		end
		else begin
			case(f)
				`ALU_F_A: begin
					y = a + cin;
					cout = y < a ? 1'b1 : 1'b0;
				end
				`ALU_F_B:y = b;
				`ALU_F_SUB:begin
					y = a - b - 1 + cin;
					cout = y > a || y > b ? 1'b0 : 1'b1;
				end
				`ALU_F_ADD:begin
					y = a + b + cin;
					cout = y < a || y < b ? 1'b1 : 1'b0;
				end
				`ALU_F_NOT:y = ~a;
				`ALU_F_XOR:y = a ^ b;
				`ALU_F_AND:y = a & b;
				`ALU_F_OR:y = a | b;
				default:$display("Warning in ALU. Unknown code at f");
			endcase
		end
		zout = y === 16'H0000 ? 1'b1 : 1'b0;
	end
endmodule

`endif

