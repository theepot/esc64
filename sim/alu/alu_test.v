`ifdef ALU_STRUCT
`include "alu_s.v"
`else
`include "alu.v"
`endif

`include "../globals.v"

module alu_test();
	reg		[15:0]  a;
	reg		[15:0]  b;
	wire	[15:0]  y;
	reg		[4:0]   f;
	reg		csel, ucin, fcin, notALUOE, notShiftOE;
	wire	cout, zout;
	
	task alu_print_state;
	begin
		$display("ERROR: alu state: a:%X b:%X y:%X f:%X csel:%X ucin:%X fcin:%X notALUOE:%X notShiftOE:%X cout:%X zout:%X\n", a, b, y, f, csel, ucin, fcin, notALUOE, notShiftOE, cout, zout);
	end
	endtask

		reg [15:0] expected_y;
		reg expected_cout;
		reg expected_zout;
		reg cin;
	
	task alu_check;
	begin

		if(csel === `ALU_CSEL_UCIN) begin
			cin = ucin;
		end 
		else begin
			cin = fcin;
		end
		if(notALUOE && notShiftOE) begin
			expected_y = 16'bzzzzzzzzzzzzzzzz;
		end
		else if(~notShiftOE && notALUOE) begin
			if(f[0]) begin
				expected_cout = a >> 15;
				expected_y = a << 1;
			end
			else begin
				expected_cout = a & 1;
				expected_y = a >> 1;
			end
		end
		else if(~notALUOE && notShiftOE) begin
			case(f)
				`ALU_F_A: begin
					expected_y = a + cin;
					expected_cout = y < a ? 1'b1 : 1'b0;
				end
				`ALU_F_B:expected_y = b;
				`ALU_F_SUB:begin
					expected_y = a - b - 1 + cin;
					expected_cout = (a - 1 + cin) < b ? 1'b0 : 1'b1;
				end
				`ALU_F_ADD:begin
					expected_y = a + b + cin;
					expected_cout = expected_y < a || expected_y < b ? 1'b1 : 1'b0;
				end
				`ALU_F_NOT:expected_y = ~a;
				`ALU_F_XOR:expected_y = a ^ b;
				`ALU_F_AND:expected_y = a & b;
				`ALU_F_OR:expected_y = a | b;
				default:$display("Warning in ALU test. Unknown code at f");
			endcase
		end
		else begin
			$display("ERROR: Testbench asserted both shitOE and ALUOE of the alu. This may not happen.");
		end
		
		expected_zout = y === 16'H0000 ? 1 : 0;
		
		if(expected_y !== y) begin
			$display("ERROR: y(result) is %X. Expected %X", y, expected_y);
			alu_print_state();
		end
		
		if((!notALUOE ^ !notShiftOE) && f !== `ALU_F_NOT && expected_cout !== cout) begin
			$display("ERROR: cout(carry out) is %X. Expected %X", cout, expected_cout);
			alu_print_state();
		end
		
		if((!notALUOE ^ !notShiftOE) && expected_zout !== zout) begin
			$display("ERROR: zout(zero out) is %X. Expected %X", zout, expected_zout);
			alu_print_state();
		end
	end
	endtask
	
	initial begin
		`ifdef ALU_STRUCT
		$dumpfile("test_s.vcd");
		`else
		$dumpfile("test.vcd");
		`endif
		$dumpvars(0);
		
		//pass through A
		$display("Testing: pass through A");
		a = 16'HDEAD;
		b = 16'HBEEF;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_A;
		
		# `TEST_DELAY alu_check();
		
		//pass through A with carry in
		$display("Testing: pass through A with carry in");
		# `TEST_DELAY a = 16'H0002;
		b = 16'HBEEF;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 1;
		f = `ALU_F_A;
		
		# `TEST_DELAY alu_check();
		
		//pass through A check zero and carry
		$display("Testing: pass through A check zero and carry");
		# `TEST_DELAY a = 16'HFFFF;
		b = 16'HBEEF;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 1;
		f = `ALU_F_A;
		
		# `TEST_DELAY alu_check();
		
		//pass through B
		$display("Testing: pass through B");
		# `TEST_DELAY a = 16'H0002;
		b = 16'HBEEF;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_A;
		
		# `TEST_DELAY alu_check();
		
		
		//add
		$display("Testing: add");
		# `TEST_DELAY a = 16'HA4D7;
		b = 16'H07F8;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_ADD;
		
		# `TEST_DELAY alu_check();
		
		//add with overflow and zero
		$display("Testing: add with overflow and zero");
		# `TEST_DELAY a = 16'HFFFF;
		b = 16'H0001;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_ADD;
		
		# `TEST_DELAY alu_check();
		
		//add with carry
		$display("Testing: add with carry");
		# `TEST_DELAY a = 16'H0010;
		b = 16'H0001;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 1;
		f = `ALU_F_ADD;
		
		# `TEST_DELAY alu_check();
		
		//subtract
		$display("Testing: subtract");
		# `TEST_DELAY a = 16'HF000;
		b = 16'H0001;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 1;
		f = `ALU_F_SUB;
		
		# `TEST_DELAY alu_check();
		
		//subtract with underflow
		$display("Testing: subtract with underflow");
		# `TEST_DELAY a = 16'H0001;
		b = 16'H0010;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 1;
		f = `ALU_F_SUB;
		
		# `TEST_DELAY alu_check();
		
		//subtract without carry
		$display("Testing: subtract without carry");
		# `TEST_DELAY a = 16'HF000;
		b = 16'H0010;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_SUB;
		
		# `TEST_DELAY alu_check();
		
		//not
		$display("Testing: not");
		# `TEST_DELAY a = 16'HF031;
		b = 16'H0010;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_NOT;
		
		# `TEST_DELAY alu_check();
		
		//xor
		$display("Testing: xor");
		# `TEST_DELAY a = 16'HF031;
		b = 16'H0010;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_XOR;
		
		# `TEST_DELAY alu_check();
		
		//and
		$display("Testing: and");
		# `TEST_DELAY a = 16'HF031;
		b = 16'H0010;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_AND;
		
		# `TEST_DELAY alu_check();
		
		//or
		$display("Testing: or");
		# `TEST_DELAY a = 16'HF031;
		b = 16'H0010;
		notALUOE = 0;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_OR;
		
		# `TEST_DELAY alu_check();
		
		//shift left
		$display("Testing: shift left");
		# `TEST_DELAY a = 16'HF031;
		b = 16'H0010;
		notALUOE = 1;
		notShiftOE = 0;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_SHIFT_LEFT;
		
		# `TEST_DELAY alu_check();
		
		//shift right
		$display("Testing: shift right");
		# `TEST_DELAY a = 16'HF031;
		b = 16'H0010;
		notALUOE = 1;
		notShiftOE = 0;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_SHIFT_RIGHT;
		
		# `TEST_DELAY alu_check();
		
		//no output
		$display("Testing: no output");
		# `TEST_DELAY a = 16'HF031;
		b = 16'H0010;
		notALUOE = 1;
		notShiftOE = 1;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_SHIFT_RIGHT;
		
		# `TEST_DELAY alu_check();
		
		//carry in from microsequencer
		$display("Testing: carry in from microsequencer");
		# `TEST_DELAY a = 16'HF031;
		b = 16'H0010;
		notALUOE = 1;
		notShiftOE = 0;
		csel = `ALU_CSEL_UCIN;
		ucin = 1;
		fcin = 0;
		f = `ALU_F_ADD;
		
		# `TEST_DELAY alu_check();
		
		#20 $finish;
	end
		
		alu koenraad_de_rekenpiraat(a, b, y, f, csel, ucin, fcin, cout, zout, notALUOE, notShiftOE);

endmodule
