#include <stdio.h>
#include <cpu.h>
#include <stdbool.h>
#include <assert.h>
#include "uassembler.h"
#include "bin_table.h"
#include <instr_info.h>

//TODO: document changes (io interface)
//TODO: y/a bus load-from/write-to enums.
//TODO: make an enum for all ALU operations and create one function that enables one of these operations

bin_table_collumn_description field_descrps[] = {
	{.name = "padding", .width = -1, .active_high = H}, //most significant bits
	{.name = "signed_cmp", .width = 1, .active_high = H},
	{.name = "at_fetch", .width = 1, .active_high = H},
	{.name = "data_reg_ld_n", .width = 1, .active_high = L},
	{.name = "address_reg_ld_n", .width = 1, .active_high = L},
	{.name = "io_select_dev", .width = 1, .active_high = H},
	{.name = "io_dir_out", .width = 1, .active_high = H},
	{.name = "io_word", .width = 1, .active_high = H},
	{.name = "io_idle_n", .width = 1, .active_high = H}, //this is an active low control signal, but it's default state should be active, so here it is active high
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
	{.name = "aluNotExtraOE", .width = 1, .active_high = L},
	{.name = "aluCSel", .width = 1, .active_high = H},
	{.name = "aluUCIn", .width = 1, .active_high = H},
	{.name = "memRead", .width = 1, .active_high = H},
	{.name = "memWrite", .width = 1, .active_high = H},
	{.name = "irNotLoad", .width = 1, .active_high = L},
	{.name = "next", .width = UROM_ADDR_WIDTH, .active_high = H},
	{.name = "nextsel", .width = 1, .active_high = H} //least significant bits
};

uassembler* u;
#define set_next(nxt)			set_next(u, nxt)
#define goto_next_free()		goto_next_free(u)
#define goto_op_entry(op, con)	goto_op_entry(u, op, con)
#define goto_fetch()			goto_fetch(u)

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
	reg_ld_sel_0 = 0,
	reg_ld_sel_1,
	reg_ld_sel_2,
	reg_ld_sel_3,
	reg_ld_sel_4,
	reg_ld_sel_sp,
	reg_ld_sel_lr,
	reg_ld_sel_pc,
	reg_ld_sel_op0,
	reg_ld_sel_ir
} reg_ld_sel;

typedef enum {
	carry_sel_zero = 0,
	carry_sel_one,
	carry_sel_status_reg
} carry_sel;

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

void reg_ld(reg_ld_sel sel)
{
	
	int regselLoadSource;
	switch(sel)
	{
	case reg_ld_sel_0 ... reg_ld_sel_pc:
		assert_signal(u, "regselLoad");
		regselLoadSource = RGS_LOADSRC_USEQ;
		set_field(u, "regselLoaduSel", sel);
		set_field(u, "regselLoadSource", regselLoadSource);
		break;
	case reg_ld_sel_op0:
		assert_signal(u, "regselLoad");
		regselLoadSource = RGS_LOADSRC_OP0;
		set_field(u, "regselLoadSource", regselLoadSource);
		break;
	case reg_ld_sel_ir:
		assert_signal(u, "irNotLoad");
		break;
	default:
		fprintf(stderr, "error: unkown reg_ld_sel\n");
		print_state(u, stderr);
		break;
	}
	
}

void carry_set(carry_sel sel)
{
	if(sel == carry_sel_status_reg)
	{
		set_field(u, "aluCSel", ALU_CSEL_SRCIN);
	}
	else
	{
		set_field(u, "aluCSel", ALU_CSEL_UCIN);
		set_field(u, "aluUCIn", sel);
	}
}

void alu_enable(int func)
{
	assert_signal(u, "aluNotALUOE");
	set_field(u, "aluF", func);
}

void shift_enable(int left_not_rigth)
{
	if(left_not_rigth)
	{
		assert_signal(u, "aluNotALUOE");
		carry_set(carry_sel_zero);
		set_field(u, "aluF", ALU_F_SHIFT_LEFT);
	}
	else
	{
		assert_signal(u, "aluNotExtraOE");
		set_field(u, "aluF", ALU_F_SHIFT_RIGHT);
	}
}

void alu_sign_extend() {
	assert_signal(u, "aluNotExtraOE");
	set_field(u, "aluF", ALU_F_SING_EXT);
}

void breg_ld(void)
{
	assert_signal(u, "aluBRegNotLoad");
}

void status_ld(void)
{
	assert_signal(u, "statusNotLoad");
}

void pc_inc(void)
{
	assert_signal(u, "pcInc");
}

void io_read(reg_ld_sel reg_dest, gpreg_oe_sel reg_with_address, bool select_device, bool word, int length)
{
	length--;
	assert_signal(u, "io_idle_n");
	if(select_device)
		assert_signal(u, "io_select_dev");
	if(word)
		assert_signal(u, "io_word");
	gpreg_oe(reg_with_address);
	assert_signal(u, "address_reg_ld_n");
	
	for(; length > 0; --length)
	{
		set_next(next_sel_next_free);
		goto_next_free();
		assert_signal(u, "io_idle_n");
		if(select_device)
			assert_signal(u, "io_select_dev");
		if(word)
			assert_signal(u, "io_word");
		assert_signal(u, "memRead");
	}
	
	set_next(next_sel_next_free);
	goto_next_free();
	assert_signal(u, "io_idle_n");
	if(select_device)
		assert_signal(u, "io_select_dev");
	if(word)
		assert_signal(u, "io_word");
	assert_signal(u, "memRead");
	reg_ld(reg_dest);
}

void io_write(gpreg_oe_sel reg_dest_addr, gpreg_oe_sel reg_src, bool select_device, bool word, int length)
{
	
	gpreg_oe(reg_src);
	alu_enable(ALU_F_A);
	assert_signal(u, "data_reg_ld_n");
	
	set_next(next_sel_next_free);
	goto_next_free();
	
	gpreg_oe(reg_dest_addr);
	assert_signal(u, "address_reg_ld_n");
	assert_signal(u, "io_idle_n");
	assert_signal(u, "io_dir_out");
	if(select_device)
		assert_signal(u, "io_select_dev");
	if(word)
		assert_signal(u, "io_word");

	
	for(; length > 0; --length)
	{
		set_next(next_sel_next_free);
		goto_next_free();
	
		assert_signal(u, "io_idle_n");
		assert_signal(u, "io_dir_out");
		if(select_device)
			assert_signal(u, "io_select_dev");
		if(word)
			assert_signal(u, "io_word");

		assert_signal(u, "memWrite");
	}
	
	set_next(next_sel_next_free);
	goto_next_free();
	
	assert_signal(u, "io_idle_n");
	assert_signal(u, "io_dir_out");
	if(select_device)
		assert_signal(u, "io_select_dev");
	if(word)
		assert_signal(u, "io_word");

	gpreg_oe(reg_dest_addr);
}

void dev_read(reg_ld_sel reg_dest, gpreg_oe_sel reg_with_address, bool word)
{
	io_read(reg_dest, reg_with_address, true, word, DEV_READ_LENGTH);
}

void dev_write(gpreg_oe_sel reg_dest_addr, gpreg_oe_sel reg_src, bool word)
{
	io_write(reg_dest_addr, reg_src, true, word, DEV_WRITE_LENGTH);
}

void mem_read(reg_ld_sel reg_dest, gpreg_oe_sel reg_with_address, bool word)
{
	io_read(reg_dest, reg_with_address, false, word, MEM_READ_LENGTH);
}

void mem_write(gpreg_oe_sel reg_dest_addr, gpreg_oe_sel reg_src, bool word)
{
	io_write(reg_dest_addr, reg_src, false, word, MEM_WRITE_LENGTH);
}

void push(gpreg_oe_sel src) {
		gpreg_oe(gpreg_oe_sel_sp);
		alu_enable(ALU_F_A_MINUS_ONE);
		carry_set(carry_sel_zero);
		reg_ld(reg_ld_sel_sp);
	set_next(next_sel_next_free);
	goto_next_free();
		gpreg_oe(gpreg_oe_sel_sp);
		alu_enable(ALU_F_A_MINUS_ONE);
		carry_set(carry_sel_zero);
		reg_ld(reg_ld_sel_sp);
	set_next(next_sel_next_free);
	goto_next_free();
	mem_write(gpreg_oe_sel_sp, src, true);
}

void error(int error_code)
{
	set_field(u, "error", error_code);
}

void expose_bit(int b)
{
	gpreg_oe(gpreg_oe_sel_0);
	breg_ld();
	set_next(next_sel_next_free);
	goto_next_free();
	gpreg_oe(gpreg_oe_sel_0);
	alu_enable(ALU_F_SUB);
	if(!b)
		carry_set(carry_sel_one);
	else
		carry_set(carry_sel_zero);
	assert_signal(u, "data_reg_ld_n");
	set_next(next_sel_next_free);
	goto_next_free();

	assert_signal(u, "io_idle_n");
	assert_signal(u, "io_dir_out");
	assert_signal(u, "io_word");
}

void create_2op_alu_instruction(opcode_t op, int alu_fun, carry_sel c)
{
	goto_op_entry(op, ALWAYS);
		breg_ld();
		gpreg_oe(gpreg_oe_sel_op2);
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(alu_fun);
		carry_set(c);
		status_ld();
		gpreg_oe(gpreg_oe_sel_op1);
		reg_ld(reg_ld_sel_op0);
	set_next(next_sel_fetch);
}

void create_nop_instruction(opcode_t op, int condition)
{
	goto_op_entry(op, condition);
	set_next(next_sel_fetch);
}

void create_conditional_mov_instruction(opcode_t op, int condition)
{
	create_nop_instruction(op, ~condition);
	goto_op_entry(op, condition);
	gpreg_oe(gpreg_oe_sel_op1);
	reg_ld(reg_ld_sel_op0);
	alu_enable(ALU_F_A);
	set_next(next_sel_fetch);
}

void create_conditional_mov_literal_instruction(opcode_t op, int condition)
{
	goto_op_entry(op, ~condition);
	pc_inc();
	set_next(next_sel_fetch);

	goto_op_entry(op, condition);
	mem_read(reg_ld_sel_op0, gpreg_oe_sel_pc, true);
	pc_inc();
	set_next(next_sel_fetch);
}

void create_illegal_instruction(opcode_t op)
{
	goto_op_entry(op, ALWAYS);
	error(ERROR_WIRE_ILLEGAL_OPCODE);
	set_next(next_sel_current);
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
		set_next(next_sel_current);
	}
}

int main(int argc, char** argv)
{
	uassembler uasm;
	u = & uasm;
	uassembler_init(u, field_descrps,  sizeof(field_descrps) / sizeof(field_descrps[0]), UROM_ADDR_WIDTH, "next", "nextsel", 7, 8*7);

	//illegal state
	fill_whole_memory_with_illegal_state();

	//illegal instructions
	fill_opcode_entrys_with_illegal_instructions();

	//reset
	//goto_reset(u);
	goto_op_entry(0, ALWAYS);
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(ALU_F_ZERO);
		reg_ld(reg_ld_sel_0);
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(ALU_F_ZERO);
		reg_ld(reg_ld_sel_1);
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(ALU_F_ZERO);
		reg_ld(reg_ld_sel_2);
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(ALU_F_ZERO);
		reg_ld(reg_ld_sel_3);
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(ALU_F_ZERO);
		reg_ld(reg_ld_sel_4);
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(ALU_F_ZERO);
		reg_ld(reg_ld_sel_sp);
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(ALU_F_ZERO);
		reg_ld(reg_ld_sel_pc);
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(ALU_F_ZERO);
		reg_ld(reg_ld_sel_lr);
		carry_set(carry_sel_zero);
		reg_ld(reg_ld_sel_ir);
		gpreg_oe(gpreg_oe_sel_sp);
		breg_ld();
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(ALU_F_A_PLUS_ONE);
		carry_set(carry_sel_one);
		gpreg_oe(gpreg_oe_sel_sp);
		status_ld();
	set_next(next_sel_fetch);

	//fetch
	goto_fetch();
	assert_signal(u, "at_fetch");
	mem_read(reg_ld_sel_ir, gpreg_oe_sel_pc, true);
	pc_inc();
	set_next(next_sel_next_free);
	goto_next_free();
	set_next(next_sel_op_entry);
	
	//mov
	create_conditional_mov_instruction(op_mov, ALWAYS);

	//mov on notcarry and notzero
	create_conditional_mov_instruction(op_movncnz, NOT_CARRY_NOT_ZERO);

	//mov on notcarry and zero
	create_conditional_mov_instruction(op_movncz, NOT_CARRY_ZERO);

	//mov on notcarry
	create_conditional_mov_instruction(op_movnc, NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on carry and notzero
	create_conditional_mov_instruction(op_movcnz, CARRY_NOT_ZERO);

	//mov on notzero
	create_conditional_mov_instruction(op_movnz, CARRY_NOT_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on notcarry or notzero
	create_conditional_mov_instruction(op_movnconz, CARRY_NOT_ZERO | NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on carry and zero
	create_conditional_mov_instruction(op_movcz, CARRY_ZERO);

	//mov on zero
	create_conditional_mov_instruction(op_movz, CARRY_ZERO | NOT_CARRY_ZERO);

	//mov on notcarry or zero
	create_conditional_mov_instruction(op_movncoz,  CARRY_ZERO | NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on carry
	create_conditional_mov_instruction(op_movc, CARRY_ZERO | CARRY_NOT_ZERO);

	//mov on carry or notzero
	create_conditional_mov_instruction(op_movconz, CARRY_ZERO | CARRY_NOT_ZERO | NOT_CARRY_NOT_ZERO);

	//mov on carry or zero
	create_conditional_mov_instruction(op_movcoz, CARRY_ZERO | CARRY_NOT_ZERO | NOT_CARRY_ZERO);

	//mov literal
	create_conditional_mov_literal_instruction(op_mov_imm, ALWAYS);

	//mov literal on notcarry and notzero
	create_conditional_mov_literal_instruction(op_movncnz_imm, NOT_CARRY_NOT_ZERO);

	//mov literal on notcarry and zero
	create_conditional_mov_literal_instruction(op_movncz_imm, NOT_CARRY_ZERO);

	//mov literal on notcarry
	create_conditional_mov_literal_instruction(op_movnc_imm, NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov literal on carry and notzero
	create_conditional_mov_literal_instruction(op_movcnz_imm, CARRY_NOT_ZERO);

	//mov literal on notzero
	create_conditional_mov_literal_instruction(op_movnz_imm, CARRY_NOT_ZERO | NOT_CARRY_NOT_ZERO);

	//mov literal on notcarry or notzero
	create_conditional_mov_literal_instruction(op_movnconz_imm, CARRY_NOT_ZERO | NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov literal on carry and zero
	create_conditional_mov_literal_instruction(op_movcz_imm, CARRY_ZERO);

	//mov literal on zero
	create_conditional_mov_literal_instruction(op_movz_imm, CARRY_ZERO | NOT_CARRY_ZERO);

	//mov literal on notcarry or zero
	create_conditional_mov_literal_instruction(op_movncoz_imm,  CARRY_ZERO | NOT_CARRY_ZERO | NOT_CARRY_NOT_ZERO);

	//mov literal on carry
	create_conditional_mov_literal_instruction(op_movc_imm, CARRY_ZERO | CARRY_NOT_ZERO);

	//mov literal on carry or notzero
	create_conditional_mov_literal_instruction(op_movconz_imm, CARRY_ZERO | CARRY_NOT_ZERO | NOT_CARRY_NOT_ZERO);

	//mov literal on carry or zero
	create_conditional_mov_literal_instruction(op_movcoz_imm, CARRY_ZERO | CARRY_NOT_ZERO | NOT_CARRY_ZERO);

	//add
	create_2op_alu_instruction(op_add, ALU_F_ADD, carry_sel_zero);

	//add with carry
	create_2op_alu_instruction(op_adc, ALU_F_ADD, carry_sel_status_reg);

	//sub
	create_2op_alu_instruction(op_sub, ALU_F_SUB, carry_sel_one);

	//sub with carry
	create_2op_alu_instruction(op_sbc, ALU_F_SUB, carry_sel_status_reg);

	//and
	create_2op_alu_instruction(op_and, ALU_F_AND, carry_sel_zero);

	//or
	create_2op_alu_instruction(op_or, ALU_F_OR, carry_sel_zero);

	//xor
	create_2op_alu_instruction(op_xor, ALU_F_XOR, carry_sel_zero);

	//compare
	goto_op_entry(op_cmp, ALWAYS);
		breg_ld();
		gpreg_oe(gpreg_oe_sel_op2);
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(ALU_F_SUB);
		status_ld();
		carry_set(carry_sel_one);
		gpreg_oe(gpreg_oe_sel_op1);
	set_next(next_sel_fetch);

	//signed compare
	goto_op_entry(op_scmp, ALWAYS);
		breg_ld();
		gpreg_oe(gpreg_oe_sel_op2);
	set_next(next_sel_next_free);
	goto_next_free();
		alu_enable(ALU_F_SUB);
		assert_signal(u, "signed_cmp");
		status_ld();
		carry_set(carry_sel_one);
		gpreg_oe(gpreg_oe_sel_op1);
	set_next(next_sel_fetch);


	//shift left
	goto_op_entry(op_shl, ALWAYS);
		gpreg_oe(gpreg_oe_sel_op1);
		reg_ld(reg_ld_sel_op0);
		shift_enable(1);
		status_ld();
	set_next(next_sel_fetch);

	//shift right
	goto_op_entry(op_shr, ALWAYS);
		gpreg_oe(gpreg_oe_sel_op1);
		reg_ld(reg_ld_sel_op0);
		shift_enable(0);
		status_ld();
	set_next(next_sel_fetch);

	//inc
	goto_op_entry(op_inc, ALWAYS);
		gpreg_oe(gpreg_oe_sel_op1);
		reg_ld(reg_ld_sel_op0);
		alu_enable(ALU_F_A_PLUS_ONE);
		carry_set(carry_sel_one);
		status_ld();
	set_next(next_sel_fetch);

	//dec
	goto_op_entry(op_dec, ALWAYS);
		gpreg_oe(gpreg_oe_sel_op1);
		reg_ld(reg_ld_sel_op0);
		alu_enable(ALU_F_A_MINUS_ONE);
		carry_set(carry_sel_zero);
		status_ld();
	set_next(next_sel_fetch);

	//not
	goto_op_entry(op_not, ALWAYS);
		gpreg_oe(gpreg_oe_sel_op1);
		reg_ld(reg_ld_sel_op0);
		alu_enable(ALU_F_NOT_A);
		status_ld();
	set_next(next_sel_fetch);

	//sign extend
	goto_op_entry(op_sxt, ALWAYS);
		gpreg_oe(gpreg_oe_sel_op1);
		alu_sign_extend();
		reg_ld(reg_ld_sel_op0);
	set_next(next_sel_fetch);


	//load word
	goto_op_entry(op_ld, ALWAYS);
	mem_read(reg_ld_sel_op0, gpreg_oe_sel_op1, true);
	set_next(next_sel_fetch);

	//load byte
	goto_op_entry(op_ldb, ALWAYS);
	mem_read(reg_ld_sel_op0, gpreg_oe_sel_op1, false);
	set_next(next_sel_fetch);

	//store word
	goto_op_entry(op_st, ALWAYS);
	mem_write(gpreg_oe_sel_op1, gpreg_oe_sel_op2, true);
	set_next(next_sel_fetch);

	//store byte
	goto_op_entry(op_stb, ALWAYS);
	mem_write(gpreg_oe_sel_op1, gpreg_oe_sel_op2, false);
	set_next(next_sel_fetch);

	//in
	goto_op_entry(op_in, ALWAYS);
	dev_read(reg_ld_sel_op0, gpreg_oe_sel_op1, true);
	set_next(next_sel_fetch);
	
	//out
	goto_op_entry(op_out, ALWAYS);
	dev_write(gpreg_oe_sel_op1, gpreg_oe_sel_op2, true);
	set_next(next_sel_fetch);

	//call
	goto_op_entry(op_call, ALWAYS);
		push(gpreg_oe_sel_pc);
	set_next(next_sel_next_free);
	goto_next_free();
		gpreg_oe(gpreg_oe_sel_op1);
		alu_enable(ALU_F_A);
		reg_ld(reg_ld_sel_pc);
	set_next(next_sel_fetch);

	//call literal
	goto_op_entry(op_call_imm, ALWAYS);
		pc_inc();
	set_next(next_sel_next_free);
	goto_next_free();
		push(gpreg_oe_sel_pc);
	set_next(next_sel_next_free);
	goto_next_free();
		gpreg_oe(gpreg_oe_sel_pc);
		alu_enable(ALU_F_A_MINUS_ONE);
		carry_set(carry_sel_zero);
		reg_ld(reg_ld_sel_pc);
	set_next(next_sel_next_free);
	goto_next_free();
		mem_read(reg_ld_sel_pc, gpreg_oe_sel_pc, true);
	set_next(next_sel_fetch);

	//push
	goto_op_entry(op_push, ALWAYS);
	push(gpreg_oe_sel_op1);
	set_next(next_sel_fetch);

	//pop
	goto_op_entry(op_pop, ALWAYS);
		mem_read(reg_ld_sel_op0, gpreg_oe_sel_sp, true);
	set_next(next_sel_next_free);
	goto_next_free();
		gpreg_oe(gpreg_oe_sel_sp);
		alu_enable(ALU_F_A_PLUS_ONE);
		carry_set(carry_sel_one);
		reg_ld(reg_ld_sel_sp);
	set_next(next_sel_next_free);
	goto_next_free();
		gpreg_oe(gpreg_oe_sel_sp);
		alu_enable(ALU_F_A_PLUS_ONE);
		carry_set(carry_sel_one);
		reg_ld(reg_ld_sel_sp);
	set_next(next_sel_fetch);
	
	
	//halt
	goto_op_entry(op_halt, ALWAYS);
	set_next(next_sel_current);

	//inspect cpu
	int f;
	int inspect_cpu_addresses[4];
	for(f = 0; f < 4; ++f)
	{
		inspect_cpu_addresses[f] = goto_next_free();

		//expose registers
		int r;
		for(r = gpreg_oe_sel_0; r < gpreg_oe_sel_0+8; ++r) {
			gpreg_oe(r);
			alu_enable(ALU_F_A);
			assert_signal(u, "data_reg_ld_n");
			set_next(next_sel_next_free);
			goto_next_free();

			assert_signal(u, "io_idle_n");
			assert_signal(u, "io_dir_out");
			assert_signal(u, "io_word");

			set_next(next_sel_next_free);
			goto_next_free();
		}

		//expose flags
		//first zero, then carry
		switch(f) {
		case 0:
			expose_bit(0);
			set_next(next_sel_next_free);
			goto_next_free();
			expose_bit(0);
			break;
		case 1:
			expose_bit(1);
			set_next(next_sel_next_free);
			goto_next_free();
			expose_bit(0);
			break;
		case 2:
			expose_bit(0);
			set_next(next_sel_next_free);
			goto_next_free();
			expose_bit(1);
			break;
		case 3:
			expose_bit(1);
			set_next(next_sel_next_free);
			goto_next_free();
			expose_bit(1);
			break;
		default:
			assert(0);
			break;
		}

		//jump to instruction(or inspect_cpu again)
		set_next(next_sel_next_free);
		goto_next_free();
		set_next(next_sel_op_entry);
	}

	int n;
	for(n = 0; n < (1 << OPCODE_WIDTH); ++n)
	{
		for(f = 0; f < 4; ++f)
		{
			goto_inspect_cpu_entry(u, n, 1 << f);
			set_next_hardcoded(u, inspect_cpu_addresses[f]);
		}
	}

	print_verilog(u, 1);

	return 0;
}
