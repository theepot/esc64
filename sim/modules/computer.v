`include "cpu.v"
`include "virtual_io.v"

`define CLOCK_PERIOD		1600
//`define MAX_CLOCK_CYCLES	1000000

module computer();
	reg [63:0] clock_counter;
	reg clock, notReset;
	reg [31:0] state;
	/* status code meaning:
	 * 0 ok
	 * 1 unknown opcode
	 * 2 hardware fault
	 * 3 reserved for future use
	 * 4 halt instr
	 * 5 read error (not implemented yet) TODO: implement read error
	 */
	
	//cpu
	wire [14:0] address;
	wire [15:0] data;
	wire rd_n, wr_n, csh_n, csl_n, select_dev;
	reg inspect_cpu;
	cpu cpu(clock, notReset, address, data, rd_n, wr_n, csh_n, csl_n, select_dev, inspect_cpu, error, at_fetch);
	
	//virtual-io
	virtual_io virtual_io(address, data, rd_n, wr_n, csh_n, csl_n, select_dev);
	
	initial begin
		$dumpfile("computer.vcd");
		$dumpvars(0);
				
		$start_sim_control();
		inspect_cpu = 0;
		clock_counter = 0;
		notReset = 0;
		clock = 0;
		state = 0;
		#900 notReset = 1;
	
`ifdef MAX_CLOCK_CYCLES
		#((`CLOCK_PERIOD / 2)*2*`MAX_CLOCK_CYCLES)
		$display("computer.v: ERROR: computer did not halt in %d cycles", `MAX_CLOCK_CYCLES);
		$finish;
`endif
	end

	always begin
		$tick(at_fetch);

		#(`CLOCK_PERIOD / 2) clock = 1'b1;
		#(`CLOCK_PERIOD / 2) clock = 1'b0;
		clock_counter = clock_counter + 1;
		
		if(ir_opcode === 7'b1111111 && clock_counter != 0) begin
			$display("computer.v: INFO: halt @ tick %d", clock_counter);
			state = 4;
		end
		else if(error !== 2'b00 && clock_counter != 0) begin
			$display("computer.v: ERROR: cpu error %d @ tick %d", cpu.error, clock_counter);
			if(^error === 1'bx) begin
				state = 2;
			end
			else begin
				state = error;
			end
		end
	end

	//monitor wires needed by SimControl extension
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
	
	wire [6:0] ir_opcode = cpu.irOpcode;
	wire [2:0] ir_op0 = cpu.regselOp0;
	wire [2:0] ir_op1 = cpu.regselOp1;
	wire [2:0] ir_op2 = cpu.regselOp2;
	
	wire [1:0] error; //connected to cpu
	
	wire at_fetch; //connected to cpu
	
	//at_fetch is high when the microsequencer it at the first microinstruction of the fetch cycle. 
	//wire at_fetch = cpu._control_unit.roms_addr[12:0] === 12'd1024 ? 1'b1 : 1'b0;
endmodule
