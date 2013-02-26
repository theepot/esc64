#include <stdio.h>
#include <cpu.h>
#include "uassembler.h"
#include "bin_table.h"

bin_table_collumn_description field_descrps[] = {
	{.name = "padding", .width = -1, .active_high = H},
	{.name = "error", .width = 2, .active_high = H},
	{.name = "statusNotLoad", .width = 1, .active_high = L},
	{.name = "regselOE", .width = 1, .active_high = H},
	{.name = "regselLoad", .width = 1, .active_high = H},
	{.name = "regselOESource", .width = 2, .active_high = H},
	{.name = "regselLoadSource", .width = 1, .active_high = H},
	{.name = "regselOEuSel", .width = 3, .active_high = H},
	{.name = "regselLoaduSel", .width = 3, .active_high = H},
	{.name = "pcInc", .width = 1, .active_high = H},
	{.name = "aluBRegNotLoad", .width = 1, .active_high = L},
	{.name = "aluNotALUOE", .width = 1, .active_high = L},
	{.name = "aluF", .width = 5, .active_high = H},
	{.name = "aluNotShiftOE", .width = 1, .active_high = L},
	{.name = "aluCSel", .width = 1, .active_high = H},
	{.name = "aluUCIn", .width = 1, .active_high = H},
	{.name = "memRead", .width = 1, .active_high = H},
	{.name = "memWrite", .width = 1, .active_high = H},
	{.name = "irNotLoad", .width = 1, .active_high = L},
	{.name = "next", .width = UROM_ADDR_WIDTH, .active_high = H},
	{.name = "nextsel", .width = 1, .active_high = H}
};

int field_descrps_n = sizeof(field_descrps) / sizeof(field_descrps[0]);
uassembler* u;

typedef enum {
	gpreg_oe_sel_0 = 0,
	gpreg_oe_sel_1,
	gpreg_oe_sel_2,
	gpreg_oe_sel_3,
	gpreg_oe_sel_4,
	gpreg_oe_sel_sp,
	gpreg_oe_sel_lr,
	gpreg_oe_sel_pc,
	gpreg_oe_sel_op0,
	gpreg_oe_sel_op1,
	gpreg_oe_sel_op2,
} gpreg_oe_sel;

typedef enum {
	gpreg_ld_sel_0 = 0,
	gpreg_ld_sel_1,
	gpreg_ld_sel_2,
	gpreg_ld_sel_3,
	gpreg_ld_sel_4,
	gpreg_ld_sel_sp,
	gpreg_ld_sel_lr,
	gpreg_ld_sel_pc,
	gpreg_ld_sel_op0,
} gpreg_ld_sel;

typedef enum {
	carry_sel_zero = 0,
	carry_sel_one,
	carry_sel_status_reg
} carry_sel;

typedef enum {
	mem_action_read,
	mem_action_write
} mem_action;

void gpreg_oe(gpreg_oe_sel sel)
{
	assert_signal(u, "regselOE");
	int regselOESource;
	switch(sel)
	{
	case gpreg_oe_sel_0 ... gpreg_oe_sel_pc:
		regselOESource = RGS_OESRC_USEQ;
		set_field(u, "regselOEuSel", sel);
		break;
	case gpreg_oe_sel_op0:
		regselOESource = RGS_OESRC_OP0;
		break;
	case gpreg_oe_sel_op1:
		regselOESource = RGS_OESRC_OP1;
		break;
	case gpreg_oe_sel_op2:
		regselOESource = RGS_OESRC_OP2;
		break;
	default:
		fprintf(stderr, "error: unkown gpreg_oe_sel\n");
		print_state(u, stderr);
		break;
	}
	set_field(u, "regselOESource", regselOESource);
}

void gpreg_ld(gpreg_ld_sel sel)
{
	assert_signal(u, "regselLoad");
	int regselLoadSource;
	switch(sel)
	{
	case gpreg_ld_sel_0 ... gpreg_ld_sel_pc:
		regselLoadSource = RGS_LOADSRC_USEQ;
		set_field(u, "regselLoaduSel", sel);
		break;
	case gpreg_ld_sel_op0:
		regselLoadSource = RGS_LOADSRC_OP0;
		break;
	default:
		fprintf(stderr, "error: unkown gpreg_ld_sel\n");
		print_state(u, stderr);
		break;
	}
	set_field(u, "regselLoadSource", regselLoadSource);
}

void alu_enable(int func)
{
	assert_signal(u, "aluNotALUOE");
	set_field(u, "aluF", func);
}

void shift_enable(int left_not_rigth)
{
	assert_signal(u, "aluNotShiftOE");
	if(left_not_rigth)
		set_field(u, "aluF", ALU_F_SHIFT_LEFT);
	else
		set_field(u, "aluF", ALU_F_SHIFT_RIGHT);
}

void breg_ld(void)
{
	assert_signal(u, "aluBRegNotLoad");
}

void status_ld(void)
{
	assert_signal(u, "statusNotLoad");
}

void ir_ld(void)
{
	assert_signal(u, "irNotLoad");
}

void carry_set(carry_sel sel)
{
	if(sel == carry_sel_status_reg)
	{
		set_field(u, "aluCSel", ALU_CSEL_FCIN);
	}
	else
	{
		set_field(u, "aluCSel", ALU_CSEL_UCIN);
		set_field(u, "aluUCIn", sel);
	}
}

void pc_inc(void)
{
	assert_signal(u, "pcInc");
}

void mem(mem_action action)
{
	if(action == mem_action_read)
	{
		assert_signal(u, "memRead");
	}
	else
	{
		assert_signal(u, "memWrite");
	}
}

void error(int error_code)
{
	set_field(u, "error", error_code);
}

void create_2op_alu_instruction(enum opcode op, int alu_fun, carry_sel c)
{
	goto_op_entry(u, op, ALWAYS);
		breg_ld();
		gpreg_oe(gpreg_oe_sel_op2);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
		alu_enable(alu_fun);
		carry_set(c);
		status_ld();
		gpreg_oe(gpreg_oe_sel_op1);
		gpreg_ld(gpreg_ld_sel_op0);
	set_next(u, next_sel_fetch);
}

void create_nop_instruction(opcode op, int condition)
{
	goto_op_entry(u, op, condition);
	set_next(u, next_sel_fetch);
}


void create_conditional_mov_instruction(opcode op, int condition)
{
	create_nop_instruction(op, ~condition);
	goto_op_entry(u, op, condition);
	gpreg_oe(gpreg_oe_sel_op1);
	gpreg_ld(gpreg_ld_sel_op0);
	alu_enable(ALU_F_A);
	set_next(u, next_sel_fetch);
}

void create_conditional_mov_literal_instruction(opcode op, int condition)
{
	//create_nop_instruction(op, ~condition);
	goto_op_entry(u, op, ~condition);
	pc_inc();
	set_next(u, next_sel_fetch);

	goto_op_entry(u, op, condition);
	gpreg_oe(gpreg_oe_sel_pc);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	pc_inc();
	gpreg_oe(gpreg_oe_sel_pc);
	mem(mem_action_read);
	gpreg_ld(gpreg_ld_sel_op0);
	set_next(u, next_sel_fetch);
}

void create_illegal_instruction(opcode op)
{
	goto_op_entry(u, op, ALWAYS);
	error(ERROR_WIRE_ILLEGAL_OPCODE);
	set_next(u, next_sel_current);
}

void fill_opcode_entrys_with_illegal_instructions(void)
{
	int n;
	for(n = 0; n < (1 << OPCODE_WIDTH); ++n)
	{
		create_illegal_instruction(n);
	}
}

void fill_whole_memory_with_illegal_state(void)
{
	int n;
	for(n = 0; n < (1 << UROM_ADDR_WIDTH); ++n)
	{
		u->current_at_op_entry = 0;
		goto_address(u, n);
		error(ERROR_WIRE_ILLEGAL_STATE);
		set_next(u, next_sel_current);
	}
}

int main(int argc, char** argv)
{
	uassembler uasm;
	u = & uasm;
	uassembler_init(u, field_descrps, field_descrps_n, UROM_ADDR_WIDTH, "next", "nextsel", 7, 8*6);

	//illegal state
	fill_whole_memory_with_illegal_state();

	//illegal instructions
	fill_opcode_entrys_with_illegal_instructions();

	//reset
	//goto_reset(u);
	goto_op_entry(u, op_reset, ALWAYS);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	alu_enable(ALU_F_ZERO);
	gpreg_ld(gpreg_ld_sel_0);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	alu_enable(ALU_F_ZERO);
	gpreg_ld(gpreg_ld_sel_1);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	alu_enable(ALU_F_ZERO);
	gpreg_ld(gpreg_ld_sel_2);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	alu_enable(ALU_F_ZERO);
	gpreg_ld(gpreg_ld_sel_3);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	alu_enable(ALU_F_ZERO);
	gpreg_ld(gpreg_ld_sel_4);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	alu_enable(ALU_F_ZERO);
	gpreg_ld(gpreg_ld_sel_sp);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	alu_enable(ALU_F_ZERO);
	gpreg_ld(gpreg_ld_sel_lr);
	carry_set(carry_sel_zero);
	ir_ld();
	gpreg_oe(gpreg_oe_sel_sp);
	breg_ld();
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	alu_enable(ALU_F_A_PLUS_ONE);
	carry_set(carry_sel_one);
	gpreg_oe(gpreg_oe_sel_sp);
	status_ld();
	set_next(u, next_sel_fetch);

	//fetch
	goto_fetch(u);
	gpreg_oe(gpreg_oe_sel_pc);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	gpreg_oe(gpreg_oe_sel_pc);
	mem(mem_action_read);
	pc_inc();
	ir_ld();
	set_next(u, next_sel_next_free);

	goto_next_free(u);
	set_next(u, next_sel_op_entry);
	
	//mov
	create_conditional_mov_instruction(op_mov, ALWAYS);

	//mov on notcarry and notzero
	create_conditional_mov_instruction(op_mov_on_notcarry_and_notzero, NOT_CARRY_NOT_ZERO);

	//mov on notcarry and zero
	create_conditional_mov_instruction(op_mov_on_notcarry_and_zero, NOT_CARRY_ZERO);

	//mov on notcarry
	create_conditional_mov_instruction(op_mov_on_notcarry, NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on carry and notzero
	create_conditional_mov_instruction(op_mov_on_carry_and_notzero, CARRY_NOT_ZERO);

	//mov on notzero
	create_conditional_mov_instruction(op_mov_on_notzero, CARRY_NOT_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on notcarry or notzero
	create_conditional_mov_instruction(op_mov_on_notcarry_or_notzero, CARRY_NOT_ZERO | NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on carry and zero
	create_conditional_mov_instruction(op_mov_on_carry_and_zero, CARRY_ZERO);

	//mov on zero
	create_conditional_mov_instruction(op_mov_on_zero, CARRY_ZERO | NOT_CARRY_ZERO);

	//mov on notcarry or zero
	create_conditional_mov_instruction(op_mov_on_notcarry_or_zero,  CARRY_ZERO | NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on carry
	create_conditional_mov_instruction(op_mov_on_carry, CARRY_ZERO | CARRY_NOT_ZERO);

	//mov on carry or notzero
	create_conditional_mov_instruction(op_mov_on_carry_or_notzero, CARRY_ZERO | CARRY_NOT_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on carry or zero
	create_conditional_mov_instruction(op_mov_on_carry_or_zero, CARRY_ZERO | CARRY_NOT_ZERO | NOT_CARRY_ZERO);

	//mov literal
	create_conditional_mov_literal_instruction(op_mov_literal, ALWAYS);

	//mov literal on notcarry and notzero
	create_conditional_mov_literal_instruction(op_mov_literal_on_notcarry_and_notzero, NOT_CARRY_NOT_ZERO);

	//mov literal on notcarry and zero
	create_conditional_mov_literal_instruction(op_mov_literal_on_notcarry_and_zero, NOT_CARRY_ZERO);

	//mov literal on notcarry
	create_conditional_mov_literal_instruction(op_mov_literal_on_notcarry, NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov literal on carry and notzero
	create_conditional_mov_literal_instruction(op_mov_literal_on_carry_and_notzero, CARRY_NOT_ZERO);

	//mov literal on notzero
	create_conditional_mov_literal_instruction(op_mov_literal_on_notzero, CARRY_NOT_ZERO | NOT_CARRY_NOT_ZERO);

	//mov literal on notcarry or notzero
	create_conditional_mov_literal_instruction(op_mov_literal_on_notcarry_or_notzero, CARRY_NOT_ZERO | NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov literal on carry and zero
	create_conditional_mov_literal_instruction(op_mov_literal_on_carry_and_zero, CARRY_ZERO);

	//mov literal on zero
	create_conditional_mov_literal_instruction(op_mov_literal_on_zero, CARRY_ZERO | NOT_CARRY_ZERO);

	//mov literal on notcarry or zero
	create_conditional_mov_literal_instruction(op_mov_literal_on_notcarry_or_zero,  CARRY_ZERO | NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov literal on carry
	create_conditional_mov_literal_instruction(op_mov_literal_on_carry, CARRY_ZERO | CARRY_NOT_ZERO);

	//mov literal on carry or notzero
	create_conditional_mov_literal_instruction(op_mov_literal_on_carry_or_notzero, CARRY_ZERO | CARRY_NOT_ZERO | NOT_CARRY_NOT_ZERO);

	//mov literal on carry or zero
	create_conditional_mov_literal_instruction(op_mov_literal_on_carry_or_zero, CARRY_ZERO | CARRY_NOT_ZERO | NOT_CARRY_ZERO);

	//add
	create_2op_alu_instruction(op_add, ALU_F_ADD, carry_sel_zero);

	//add with carry
	create_2op_alu_instruction(op_add_with_carry, ALU_F_ADD, carry_sel_status_reg);

	//sub
	create_2op_alu_instruction(op_sub, ALU_F_SUB, carry_sel_one);

	//sub with carry
	create_2op_alu_instruction(op_sub_with_carry, ALU_F_SUB, carry_sel_status_reg);

	//and
	create_2op_alu_instruction(op_and, ALU_F_AND, carry_sel_zero);

	//or
	create_2op_alu_instruction(op_or, ALU_F_OR, carry_sel_zero);

	//xor
	create_2op_alu_instruction(op_xor, ALU_F_XOR, carry_sel_zero);

	//compare
	goto_op_entry(u, op_cmp, ALWAYS);
	breg_ld();
	gpreg_oe(gpreg_oe_sel_op2);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	alu_enable(ALU_F_SUB);
	status_ld();
	carry_set(carry_sel_one);
	gpreg_oe(gpreg_oe_sel_op1);
	set_next(u, next_sel_fetch);

	//shift left
	goto_op_entry(u, op_shift_left_1, ALWAYS);
	gpreg_oe(gpreg_oe_sel_op1);
	gpreg_ld(gpreg_ld_sel_op0);
	shift_enable(1);
	status_ld();
	set_next(u, next_sel_fetch);

	//shift right
	goto_op_entry(u, op_shift_right_1, ALWAYS);
	gpreg_oe(gpreg_oe_sel_op1);
	gpreg_ld(gpreg_ld_sel_op0);
	shift_enable(0);
	status_ld();
	set_next(u, next_sel_fetch);

	//inc
	goto_op_entry(u, op_inc, ALWAYS);
	gpreg_oe(gpreg_oe_sel_op1);
	gpreg_ld(gpreg_ld_sel_op0);
	alu_enable(ALU_F_A_PLUS_ONE);
	carry_set(carry_sel_one);
	status_ld();
	set_next(u, next_sel_fetch);

	//dec
	goto_op_entry(u, op_dec, ALWAYS);
	gpreg_oe(gpreg_oe_sel_op1);
	gpreg_ld(gpreg_ld_sel_op0);
	alu_enable(ALU_F_A_MINUS_ONE);
	carry_set(carry_sel_zero);
	status_ld();
	set_next(u, next_sel_fetch);

	//not
	goto_op_entry(u, op_not, ALWAYS);
	gpreg_oe(gpreg_oe_sel_op1);
	gpreg_ld(gpreg_ld_sel_op0);
	alu_enable(ALU_F_NOT_A);
	status_ld();
	set_next(u, next_sel_fetch);


	//load
	goto_op_entry(u, op_load, ALWAYS);
		gpreg_oe(gpreg_oe_sel_op1);
		set_next(u, next_sel_next_free);
	goto_next_free(u);
		gpreg_oe(gpreg_oe_sel_op1);
		mem(mem_action_read);
		gpreg_ld(gpreg_ld_sel_op0);
	set_next(u, next_sel_fetch);

	//store
	goto_op_entry(u, op_store, ALWAYS);
		gpreg_oe(gpreg_oe_sel_op2);
		breg_ld();
		set_next(u, next_sel_next_free);
	goto_next_free(u);
		alu_enable(ALU_F_B);
		gpreg_oe(gpreg_oe_sel_op1);
		set_next(u, next_sel_next_free);
	goto_next_free(u);
		alu_enable(ALU_F_B);
		gpreg_oe(gpreg_oe_sel_op1);
		mem(mem_action_write);
		set_next(u, next_sel_next_free);
	goto_next_free(u);
		alu_enable(ALU_F_B);
		gpreg_oe(gpreg_oe_sel_op1);
		set_next(u, next_sel_fetch);

	//in
	goto_op_entry(u, op_in, ALWAYS);
	gpreg_oe(gpreg_oe_sel_op1);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	{
		int n;
		for(n = 0; n < IN_INSTRUCTION_DELAY; ++n)
		{
			gpreg_oe(gpreg_oe_sel_op1);
			mem(mem_action_read);
			set_next(u, next_sel_next_free);
			goto_next_free(u);
		}
	}
	gpreg_oe(gpreg_oe_sel_op1);
	mem(mem_action_read);
	gpreg_ld(gpreg_ld_sel_op0);
	set_next(u, next_sel_fetch);

	//out
	goto_op_entry(u, op_out, ALWAYS);
		gpreg_oe(gpreg_oe_sel_op2);
		breg_ld();
		set_next(u, next_sel_next_free);
	goto_next_free(u);
		alu_enable(ALU_F_B);
		gpreg_oe(gpreg_oe_sel_op1);
		set_next(u, next_sel_next_free);
	goto_next_free(u);
	{
		int n;
		for(n = 0; n < OUT_INSTRUCTION_DELAY; ++n)
		{
			alu_enable(ALU_F_B);
			gpreg_oe(gpreg_oe_sel_op1);
			mem(mem_action_write);
			set_next(u, next_sel_next_free);
			goto_next_free(u);
		}
	}
	alu_enable(ALU_F_B);
	gpreg_oe(gpreg_oe_sel_op1);
	set_next(u, next_sel_fetch);


	//call
	goto_op_entry(u, op_call, ALWAYS);
	gpreg_oe(gpreg_oe_sel_pc);
	alu_enable(ALU_F_A);
	gpreg_ld(gpreg_ld_sel_lr);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	gpreg_oe(gpreg_oe_sel_op1);
	alu_enable(ALU_F_A);
	gpreg_ld(gpreg_ld_sel_pc);
	set_next(u, next_sel_fetch);

	//call literal
	goto_op_entry(u, op_call_literal, ALWAYS);
	gpreg_oe(gpreg_oe_sel_pc);
	alu_enable(ALU_F_A_PLUS_ONE);
	carry_set(carry_sel_one);
	gpreg_ld(gpreg_ld_sel_lr);
	set_next(u, next_sel_next_free);
	goto_next_free(u);
	gpreg_oe(gpreg_oe_sel_pc);
	mem(mem_action_read);
	gpreg_ld(gpreg_ld_sel_pc);
	set_next(u, next_sel_fetch);

	//push
	goto_op_entry(u, op_push, ALWAYS);
		gpreg_oe(gpreg_oe_sel_op1);
		breg_ld();
		set_next(u, next_sel_next_free);
	goto_next_free(u);
		alu_enable(ALU_F_B);
		gpreg_oe(gpreg_oe_sel_sp);
		set_next(u, next_sel_next_free);
	goto_next_free(u);
		alu_enable(ALU_F_B);
		gpreg_oe(gpreg_oe_sel_sp);
		mem(mem_action_write);
		set_next(u, next_sel_next_free);
	goto_next_free(u);//TODO: this uop might not be necessary
		alu_enable(ALU_F_B);
		gpreg_oe(gpreg_oe_sel_sp);
		set_next(u, next_sel_next_free);
	goto_next_free(u);
		gpreg_oe(gpreg_oe_sel_sp);
		alu_enable(ALU_F_A_MINUS_ONE);
		carry_set(carry_sel_zero);
		gpreg_ld(gpreg_ld_sel_sp);
		set_next(u, next_sel_fetch);

	//pop
	goto_op_entry(u, op_pop, ALWAYS);
		gpreg_oe(gpreg_oe_sel_sp);
		alu_enable(ALU_F_A_PLUS_ONE);
		carry_set(carry_sel_one);
		gpreg_ld(gpreg_ld_sel_sp);
		set_next(u, next_sel_next_free);
	goto_next_free(u);
		gpreg_oe(gpreg_oe_sel_sp);
		set_next(u, next_sel_next_free);
	goto_next_free(u);
		gpreg_oe(gpreg_oe_sel_sp);
		mem(mem_action_read);
		gpreg_ld(gpreg_ld_sel_op0);
	set_next(u, next_sel_fetch);


	//halt
	goto_op_entry(u, op_halt, ALWAYS);
	set_next(u, next_sel_current);

	print_verilog(u, 1);

	return 0;
}
