`include "mSeq.v"

`include "../globals.v"

`define TEST_DELAY (500)
`define TEST_CLOCK (800)

module mSeq_test();
	reg clock, notReset, carry, zero;
	reg [1:0] opcode;
	wire [9:0] control;
	
	initial begin
		$dumpfile("test.vcd");
		
		$dumpvars(0, clock, notReset, carry, zero, opcode, control, microsequencer.address_register_output, microsequencer.rom_data, microsequencer.rom_data_sel, microsequencer.rom_data_next);
	
		clock = 0;
		notReset = 1;
		carry = 0;
		zero = 0;
		opcode = 0;
		
		#10 notReset = 0;
		#20 notReset = 1;
		
		#800 $display("warning: finished by timeout timer");
		$finish;
	end
	
	always @ (posedge control[0]) begin
		opcode = opcode + 1;
		if(opcode == 3) begin
			$finish;
		end
	end
	
	always begin
		#10 clock = ~clock;
	end
	
	mSeq #(.OPCODE_WIDTH(2), .ADDR_WIDTH(5), .CONTROL_WIDTH(10), .INITIAL_ADDRESS(16), .ROM_FILENAME("urom.lst")) microsequencer(clock, notReset, opcode, carry, zero, control);

endmodule
