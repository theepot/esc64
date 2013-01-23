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
	{.name = "memNotRead", .width = 1, .active_high = L},
	{.name = "memNotWrite", .width = 1, .active_high = L},
	//{.name = "dummy", .width = 1, .active_high = L},
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
	gpreg_oe_sel_5,
	gpreg_oe_sel_6,
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
	gpreg_ld_sel_5,
	gpreg_ld_sel_6,
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
		assert_signal(u, "memNotRead");
	}
	else
	{
		assert_signal(u, "memNotWrite");
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
	goto_next(u);
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

void create_illegal_instruction(opcode op)
{
	goto_op_entry(u, op, ALWAYS);
	error(ERROR_WIRE_ILLEGAL_OPCODE);
	set_next(u, next_sel_current);
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
	goto_reset(u);
	set_next(u, next_sel_fetch);

	//fetch
	goto_fetch(u);
	gpreg_oe(gpreg_oe_sel_pc);
	mem(mem_action_read);
	pc_inc();
	ir_ld();
	set_next(u, next_sel_next_free);

	goto_next(u);
	set_next(u, next_sel_op_entry);
	
	//mov
	create_conditional_mov_instruction(op_mov, ALWAYS);

	//mov on zero
	create_conditional_mov_instruction(op_mov_on_zero, CARRY_ZERO | NOT_CARRY_ZERO);

	//mov on not zero
	create_conditional_mov_instruction(op_mov_on_not_zero, CARRY_NOT_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on not carry
	create_conditional_mov_instruction(op_mov_on_not_carry, NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on not carry or zero
	create_conditional_mov_instruction(op_mov_on_not_carry_or_zero, NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO | CARRY_ZERO);

	//add
	create_2op_alu_instruction(op_add, ALU_F_ADD, carry_sel_zero);

	//sub
	create_2op_alu_instruction(op_sub, ALU_F_SUB, carry_sel_one);

	//and
	create_2op_alu_instruction(op_and, ALU_F_AND, carry_sel_zero);

	//or
	create_2op_alu_instruction(op_or, ALU_F_OR, carry_sel_zero);

	//xor
	create_2op_alu_instruction(op_xor, ALU_F_XOR, carry_sel_zero);

	//shift left
	goto_op_entry(u, op_shift_left, ALWAYS);
	gpreg_oe(gpreg_oe_sel_op1);
	gpreg_ld(gpreg_ld_sel_op0);
	shift_enable(1);
	status_ld();
	set_next(u, next_sel_fetch);

	//shift right
	goto_op_entry(u, op_shift_right, ALWAYS);
	gpreg_oe(gpreg_oe_sel_op1);
	gpreg_ld(gpreg_ld_sel_op0);
	shift_enable(0);
	status_ld();
	set_next(u, next_sel_fetch);

	//mov literal
	goto_op_entry(u, op_mov_literal, ALWAYS);
	pc_inc();
	gpreg_oe(gpreg_oe_sel_pc);
	mem(mem_action_read);
	gpreg_ld(gpreg_ld_sel_op0);
	set_next(u, next_sel_fetch);

	//load
	goto_op_entry(u, op_load, ALWAYS);
	gpreg_oe(gpreg_oe_sel_op1);
	mem(mem_action_read);
	gpreg_ld(gpreg_ld_sel_op0);
	set_next(u, next_sel_fetch);

	//store
	goto_op_entry(u, op_store, ALWAYS);
	gpreg_oe(gpreg_oe_sel_op2);
	breg_ld();
	set_next(u, next_sel_next_free);
	goto_next(u);
	alu_enable(ALU_F_B);
	gpreg_oe(gpreg_oe_sel_op1);
	mem(mem_action_write);
	set_next(u, next_sel_next_free);
	goto_next(u);
	gpreg_oe(gpreg_oe_sel_op1);
	set_next(u, next_sel_fetch);

	//compare
	goto_op_entry(u, op_cmp, ALWAYS);
	breg_ld();
	gpreg_oe(gpreg_oe_sel_op2);
	set_next(u, next_sel_next_free);
	goto_next(u);
	alu_enable(ALU_F_SUB);
	status_ld();
	gpreg_oe(gpreg_oe_sel_op1);
	set_next(u, next_sel_fetch);

	//call
	goto_op_entry(u, op_call, ALWAYS);
	gpreg_oe(gpreg_oe_sel_pc);
	alu_enable(ALU_F_A);
	gpreg_ld(gpreg_ld_sel_6);
	set_next(u, next_sel_next_free);
	goto_next(u);
	gpreg_oe(gpreg_oe_sel_op1);
	alu_enable(ALU_F_A);
	gpreg_ld(gpreg_ld_sel_pc);
	set_next(u, next_sel_fetch);



	print_verilog(u, 1);

/*	//ucode
	//init
	put_op_entry("", 0, dontcare, dontcare, fetch, mem);

	//fetch
	put_uop("regselOE, regselOESource="RGS_OESRC_USEQ", regselOEuSel="RGS_PC", memNotCS, memNotRead, irNotLoad, pcInc", next, mem);
	put_uop("", op_entry, mem);
	
	//mov's
#define MOV(o,c,z) put_op_entry("regselOE, regselOESource="RGS_OESRC_OP1", regselLoad, regselLoadSource="RGS_LOADSRC_OP0", aluNotALUOE, aluF="ALU_F_A, (o), (c), (z), fetch, mem)
#define NOP(o) put_op_entry("", (o), dontcare, dontcare, fetch, mem)

	MOV(op_mov, dontcare, dontcare);
	
	NOP(op_mov_on_zero);
	MOV(op_mov_on_zero, dontcare, true);
	
	NOP(op_mov_on_not_zero);
	MOV(op_mov_on_not_zero, dontcare, false);
	
	NOP(op_mov_on_not_carry);
	MOV(op_mov_on_not_carry, false, dontcare);
	
	NOP(op_mov_on_not_carry_or_zero);
	MOV(op_mov_on_not_carry_or_zero, false, dontcare);
	MOV(op_mov_on_not_carry_or_zero, dontcare, true);
	
	//mov immediate
	put_op_entry("pcInc, regselLoad, regselLoadSource="RGS_LOADSRC_OP0", regselOE, regselOESource="RGS_OESRC_USEQ", regselOEuSel="RGS_PC", memNotCS, memNotRead", op_mov_literal, dontcare, dontcare, fetch, mem);
	
	//ALU functions
#define FN(op, f) \
	put_op_entry("regselOE, regselOESource="RGS_OESRC_OP2", aluBRegNotLoad", (op), dontcare, dontcare, next, mem); \
	put_uop("regselOE, regselOESource="RGS_OESRC_OP1", regselLoad, regselLoadSource="RGS_LOADSRC_OP0", aluNotALUOE, statusNotLoad, aluF="  f ", aluCSel="ALU_CSEL_UCIN, fetch, mem)
	FN(op_add, ALU_F_ADD);
	FN(op_or, ALU_F_OR);
	FN(op_xor, ALU_F_XOR);
	FN(op_and, ALU_F_AND);
	put_op_entry("regselOE, regselOESource="RGS_OESRC_OP2", aluBRegNotLoad", op_sub, dontcare, dontcare, next, mem); \
	put_uop("regselOE, regselOESource="RGS_OESRC_OP1", regselLoad, regselLoadSource="RGS_LOADSRC_OP0", statusNotLoad, aluNotALUOE, aluF="ALU_F_SUB", aluCSel="ALU_CSEL_UCIN", aluUCIn", fetch, mem);
	
	//shift left
	put_op_entry("regselOE, regselOESource="RGS_OESRC_OP1", regselLoad, regselLoadSource="RGS_LOADSRC_OP0", aluNotShiftOE, statusNotLoad, aluF="ALU_F_SHIFT_LEFT, op_shift_left, dontcare, dontcare, fetch, mem);
	
	//shift right
	put_op_entry("regselOE, regselOESource="RGS_OESRC_OP1", regselLoad, regselLoadSource="RGS_LOADSRC_OP0", aluNotShiftOE, statusNotLoad, aluF="ALU_F_SHIFT_RIGHT, op_shift_right, dontcare, dontcare, fetch, mem);
	
	
	//load
	put_op_entry("regselOE, regselOESource="RGS_OESRC_OP1", regselLoad, regselLoadSource="RGS_LOADSRC_OP0", memNotCS, memNotRead", op_load, dontcare, dontcare, fetch, mem);
	
	//store
	put_op_entry("regselOE, regselOESource="RGS_OESRC_OP2", aluBRegNotLoad, aluF="ALU_F_B", aluNotALUOE", op_store, dontcare, dontcare, next, mem);
	put_uop("memNotWrite, memNotCS, regselOE, regselOESource="RGS_OESRC_OP1", aluF="ALU_F_B", aluNotALUOE", next, mem);
	put_uop("memNotCS, regselOE, regselOESource="RGS_OESRC_OP1", aluF="ALU_F_B", aluNotALUOE", fetch, mem);
	
	//comp
	put_op_entry("regselOE, regselOESource="RGS_OESRC_OP2", aluBRegNotLoad", op_cmp, dontcare, dontcare, next, mem); \
	put_uop("regselOE, regselOESource="RGS_OESRC_OP1", statusNotLoad, aluNotALUOE, aluF="ALU_F_SUB", aluCSel="ALU_CSEL_UCIN", aluUCIn", fetch, mem);
	
	//call
	put_op_entry("aluNotALUOE, regselLoad, regselLoadSource="RGS_LOADSRC_USEQ", regselLoaduSel="RGS_LR", regselOE, regselOESource="RGS_OESRC_USEQ", regselOEuSel="RGS_PC, op_call, dontcare, dontcare, next, mem);
	put_uop("aluNotALUOE, regselOE, regselOESource="RGS_OESRC_OP1", regselLoad, regselLoadSource="RGS_LOADSRC_USEQ", regselLoaduSel="RGS_PC, fetch, mem);
	
	*/

	//print
	return 0;
}
