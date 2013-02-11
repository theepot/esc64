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
		
		//cpu.status.r.out = 0;
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
			#5 $writememb("ram_out.lst", ram.mem, 0, (1<<15)-1);
			$finish;
		end
	end
	
	//error monitor wires
	wire [1:0] error = cpu.error;
	
	//ISA monitor wires
	wire [15:0] reg0 = cpu.registers.r[0].content;
	wire [15:0] reg1 = cpu.registers.r[1].content;
	wire [15:0] reg2 = cpu.registers.r[2].content;
	wire [15:0] reg3 = cpu.registers.r[3].content;
	wire [15:0] reg4 = cpu.registers.r[4].content;
	wire [15:0] reg5 = cpu.registers.r[5].content;
	wire [15:0] reg6 = cpu.registers.r[6].content;
	wire [15:0] pc = cpu.registers.pc.content;
	
	wire carry = cpu.statusCOut;
	wire zero = cpu.statusZOut;
	
	wire [6:0] opcode = cpu.irOpcode;
	wire [2:0] op0 = cpu.regselOp0;
	wire [2:0] op1 = cpu.regselOp1;
	wire [2:0] op2 = cpu.regselOp2;

endmodule
