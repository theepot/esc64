`include "ALU.v"
module ALU_test();
	reg		[15:0]  a;
	reg		[15:0]  b;
	wire	[15:0]  y;
	reg		[4:0]   f;
	reg		fsel, csel, ucin, fcin;
	wire	cout, zout;
	
	initial begin
		$monitor("time: %d\na: %X\nb: %X\ny: %X\nf: %b\nfsel: %d\ncsel: %d\nucin: %d\nfcin: %d\ncout: %d\nzout: %d\n",
			$time, a, b, y, f, fsel, csel, ucin, fcin, cout, zout);
		
		$dumpfile("wave.vcd");
		$dumpvars(0, a, b, y, f, fsel, csel, ucin, fcin, cout, zout);
		
		a = 0;
		b = 0;
		fsel = `ALU_FSEL_74181;
		csel = `ALU_CSEL_FCIN;
		ucin = 0;
		fcin = 0;
		f = `ALU_F_A;
		
		//pass through
		a = 16'HDEAD;
		b = 16'HBEEF;
		
		//add
		#10 f = `ALU_F_ADD;
		a = 16'H0F00;
		b = 16'H0010;
		
		#5 a = 16'HFFFF;
		b = 16'H0002;
		
		#5 fcin = 1;
		
		//subtract
		#10 f = `ALU_F_SUB;
		csel = `ALU_CSEL_UCIN;
		ucin = 1;
		a = 16'H009;
		b = 16'H009;
		
		#5 b = 16'H007;
		#5 b = 16'H00A;
		#5 ucin = 0;
		
		//not
		#10 f = `ALU_F_NOT;
		a = 16'HF0F0;
		
		//xor
		#10 f = `ALU_F_XOR;
		a = 16'b0000000000001001;
		b = 16'b0000000000000101;
		
		//and
		#10 f = `ALU_F_AND;
		a = 16'b0000000000001001;
		b = 16'b0000000000000101;
		
		//or
		#10 f = `ALU_F_OR;
		a = 16'b0000000000001001;
		b = 16'b0000000000000101;
		
		//shift left
		#10 fsel = `ALU_FSEL_SHIFT;
		f = `ALU_F_SHIFT_LEFT;
		a = 16'HF00F;
		b = 16'H0005;
		
		//shift right
		#10 f = `ALU_F_SHIFT_RIGHT;
		a = 16'HF00F;
		b = 16'H000F;
		
		#20 $finish;
	end
		
		ALU alu(a, b, y, f, fsel, csel, ucin, fcin, cout, zout);

endmodule
