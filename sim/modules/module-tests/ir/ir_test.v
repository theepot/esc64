`ifdef IR_STRUCT
`include "ir_s.v"
`define DUMPNAME "ir_s.vcd"
`else
`include "ir.v"
`define DUMPNAME "ir.vcd"
`endif

`include "../globals.v"
`define TEST_DELAY (500)
`define TEST_CLOCK (800)

module ir_test();
	reg [15:0] data;
	reg dataE;
	wire [15:0] data_bus;
	
	wire [6:0] opcode;
	wire [2:0] op0, op1, op2;
	
	reg clock, notLoad;
	
	assign data_bus = dataE ? data : 16'bzzzzzzzzzzzzzzzz;
	
	initial begin
		$dumpfile(`DUMPNAME);
		$dumpvars(0, data, opcode, op0, op1, op2, clock, notLoad);
	
		data = 0;
		dataE = 0;
		
		clock = 0;
		notLoad = 1;
		
		#100	data = 16'b1010111_101_110_011;
		#100	dataE = 1;
		#100	notLoad = 0;
		#100	notLoad = 1;
		#100	dataE = 0;
		
		if(opcode !== 7'b1010111) begin
			$display("ERROR: opcode=%d (should be 0b1010111)\n", opcode);
		end
		
		if(op0 !== 3'b101) begin
			$display("ERROR: ir: op0=%d (should be 0b101)\n", opcode);
		end
		
		if(op1 !== 3'b110) begin
			$display("ERROR: ir: op1=%d (should be 0b110)\n", opcode);
		end
		
		if(op2 !== 3'b011) begin
			$display("ERROR: ir: op2=%d (should be 0b011)\n", opcode);
		end
		
		#100	$finish;
	end
	
	always begin
		#20	clock = ~clock;
	end
	
	instructionRegister ir(clock, notLoad, data, opcode, op0, op1, op2);

endmodule
