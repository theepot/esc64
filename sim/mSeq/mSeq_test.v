`include "mSeq.v"

module mSeq_test();
	reg clock, reset, carry, zero;
	reg [1:0] opcode;
	wire [1:0] control;
	
	initial begin
		$dumpfile("wave.vcd");
		$dumpvars(0, clock, reset, carry, zero, opcode, control, microsequencer.addres_register_output, microsequencer.rom_data, microsequencer.rom_data_sel, microsequencer.rom_data_next);
	
		clock = 0;
		reset = 0;
		carry = 1;
		zero = 1;
		opcode = 0;
		
		#10 reset = 1;
		#20 reset = 0;
		
		#200 $finish;
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
	
	mSeq #(.OPCODE_WIDTH(2), .ADDR_WIDTH(5), .CONTROL_WIDTH(2), .INITIAL_ADDRESS(16), .ROM_FILENAME("urom.lst")) microsequencer(clock, reset, opcode, carry, zero, control);

endmodule
