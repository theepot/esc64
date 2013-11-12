`ifdef ALL_STRUCT
	`define GPREGISTER_STRUCT 1
	`define PC_STRUCT 1
	`define STATUS_STRUCT 1
	`define IR_STRUCT 1
	`define ALU_STRUCT 1
	`define REGSEL_STRUCT 1
	`define BREG_STRUCT 1
	`define MSEQ_STRUCT 1
	`define SRAM_STRUCT 1
`endif

`ifdef SRAM_STRUCT
`include "../sram/sram_s.v"
`else
`include "../sram/sram.v"
`endif

`include "../cpu/cpu.v"

`define CLOCK_PERIOD		1600
`define MAX_CLOCK_CYCLES	100000

module computer();
	reg [63:0] tick_counter;
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
				
		//$print_add(10, 20);
		$start_thrift_server();
		
		//cpu.status.r.out = 0;
		tick_counter = 0;
		notReset = 0;
		clock = 0;
		#900 notReset = 1;

		#((`CLOCK_PERIOD / 2)*2*`MAX_CLOCK_CYCLES)
		$display("ERROR: computer did not halt in %d cycles", `MAX_CLOCK_CYCLES);
		$finish;

	end

	always begin
		$tick;
	
		if(cpu.irOpcode === 7'b1111111 && tick_counter != 0) begin
			$display("halt @ tick %d", tick_counter);
			$display("dumping ram");
			#5 $writememb("ram_out.lst", ram.mem, 0, (1<<15)-1);
			$halt;
			//$finish;
		end
		else if(cpu.error !== 2'b00 && tick_counter != 0) begin
			$display("error %d @ tick %d", cpu.error, tick_counter);
			//$finish;
			$error(cpu.error);
		end
		else begin
			#(`CLOCK_PERIOD / 2) clock = 1'b1;
			#(`CLOCK_PERIOD / 2) clock = 1'b0;
			tick_counter = tick_counter + 1;
		end
	end

	/*always begin
		$tick;
		
		//FIXME debug
		$display("tick=%d", tick);
		tick = tick + 1;
		//end debug
		
		#(`CLOCK_PERIOD / 2) clock = ~clock;
		tick_counter = tick_counter + 1;
		if(cpu.irOpcode === 7'b1111111) begin
			$display("halt @ tick %d", tick_counter / 2);
			$display("dumping ram");
			#5 $writememb("ram_out.lst", ram.mem, 0, (1<<15)-1);
			$finish;
		end
		if(cpu.error !== 2'b00) begin
			$display("error %d @ tick %d", cpu.error, tick_counter / 2);
			$finish;
		end
	end*/
	
	always @ (posedge clock) begin
		ram.test_state();
	end
	
	//ISA monitor wires
	wire [15:0] reg0 = cpu.registers.r[0].content;
	wire [15:0] reg1 = cpu.registers.r[1].content;
	wire [15:0] reg2 = cpu.registers.r[2].content;
	wire [15:0] reg3 = cpu.registers.r[3].content;
	wire [15:0] reg4 = cpu.registers.r[4].content;
	wire [15:0] reg5 = cpu.registers.r[5].content;
	wire [15:0] reg6 = cpu.registers.r[6].content;
	wire [15:0] reg7 = cpu.registers.pc.content;
	wire [15:0] pc = cpu.registers.pc.content;
	
	wire carry = cpu.statusCOut;
	wire zero = cpu.statusZOut;
	
	wire [6:0] opcode = cpu.irOpcode;
	wire [2:0] op0 = cpu.regselOp0;
	wire [2:0] op1 = cpu.regselOp1;
	wire [2:0] op2 = cpu.regselOp2;
	
	wire at_fetch = cpu._mSeq.roms_addr[12:0] === 12'd512 ? 1'b1 : 1'b0;
endmodule
