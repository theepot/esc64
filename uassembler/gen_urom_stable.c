#include "uassembler.h"
#include "bin_table.h"
//#include "../cpu/cpu.h"

#define UROM_ADDR_WIDTH 13

bin_table_collumn_description field_descrps[] = {
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
	{.name = "irNotLoad", .width = 1, .active_high = L}
	{.name = "next", .width = UROM_ADDR_WIDTH, .active_high = H},
	{.name = "nextsel", .width = 1, .active_high_high = H}
};

int main(int argc, char** argv)
{
	uassembler uasm;
	uassembler_init(&uasm, field_descr, , UROM_ADDR_WIDTH, "next", "nextsel", 7, 512);
	
	//ucode
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
	/*sub*/put_op_entry("regselOE, regselOESource="RGS_OESRC_OP2", aluBRegNotLoad", op_sub, dontcare, dontcare, next, mem); \
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
	
	//print
	print_mem_verilog_bin(mem, mem_size, 1);
	return 0;
}
