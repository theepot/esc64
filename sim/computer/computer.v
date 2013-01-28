`ifdef ALL_STRUCT
	`define GPREGISTER_STRUCT 1
	`define PC_STRUCT 1
	`define STATUS_STRUCT 1
	`define IR_STRUCT 1
	`define ALU_STRUCT 1
	`define REGSEL_STRUCT 1
	`define BREG_STRUCT 1
	`define MSEQ_STRUCT 1
`endif

`include "../cpu/cpu.v"
`include "../sram/sram.v"

`define CLOCK_PERIOD 1600

module computer();
	reg clock, notReset;
	
	//cpu
	wire [15:0] address, data;
	wire memNotRead, memNotWrite;
	cpu cpu(clock, notReset, address, data, memNotRead, memNotWrite);
	
	//ram
	sram #(.MEMFILE("ram.lst")) ram({1'b0, address[14:0]}, data, memNotRead, memNotWrite, address[15]);
	
	initial begin
		$dumpfile("computer.vcd");
		$dumpvars(0);
		
		notReset = 0;
		clock = 0;
		#900 notReset = 1;

		
		#((`CLOCK_PERIOD / 2)*2*10000)
		$display("ERROR: computer did not halt in 10000 cycles");
		$finish;

	end

	always begin
		#(`CLOCK_PERIOD / 2) clock = ~clock;
		if(cpu.irOpcode == 7'b1111111) begin
			$display("halt");
			//#5 $writememb("memBottomDump.lst", ram.mem, 0, 64);
			//$writememb("memTopDump.lst", ram.mem, (1<<15) - 100, (1<<15)-1);
			$finish;
		end
	end

endmodule
