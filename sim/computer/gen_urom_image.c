#include <uassembler.h>
#include "../cpu/cpu.h"

field_description field_descrps[] = {
	{.name = "nextsel", .index = 0, .width = 1, .active = 1},
	{.name = "next", .index = 1, .width = UROM_ADDR_WIDTH, .active = 1},
	{.name = "statusNotLoad", .index = (26 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = L},
	{.name = "regselOE", .index = (25 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = H},
	{.name = "regselLoad", .index = (24 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = H},
	{.name = "regselOESourceSel", .index = (22 + 1 + UROM_ADDR_WIDTH), .width = (23-22+1), .active = H},
	{.name = "regselLoadSourceSel", .index = (21 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = H},
	{.name = "regselUSeqRegSelOE", .index = (18 + 1 + UROM_ADDR_WIDTH), .width = (20-18+1), .active = H},
	{.name = "regselUSeqRegSelLoad", .index = (15 + 1 + UROM_ADDR_WIDTH), .width = (17-15+1), .active = H},
	{.name = "pcInc", .index = (14 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = H},
	{.name = "aluBRegNotLoad", .index = (13 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = L},
	{.name = "aluNotALUOE", .index = (12 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = L},
	{.name = "aluF", .index = (7 + 1 + UROM_ADDR_WIDTH), .width = (11-7+1), .active = H},
	{.name = "aluNotShiftOE", .index = (6 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = L},
	{.name = "aluCSel", .index = (5 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = H},
	{.name = "aluUCIn", .index = (4 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = H},
	{.name = "memNotRead", .index = (3 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = L},
	{.name = "memNotWrite", .index = (2 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = L},
	{.name = "memNotCS", .index = (1 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = L}, //this line is not used anymore
	{.name = "irNotLoad", .index = (0 + 1 + UROM_ADDR_WIDTH), .width = 1, .active = L}
};

int main(int argc, char** argv)
{
	field_descriptions = field_descrps;
	fields_amount = sizeof(field_descrps) / sizeof(field_description);
	addr_width = UROM_ADDR_WIDTH;
	data_width = UROM_DATA_WIDTH;
	opcode_width = OPCODE_WIDTH;
	
	init();
	fetch_index = pow(2, opcode_width + 2);
	next_uop_index = fetch_index;
	
	//ucode
	//init
	put_op_entry("", 0, dontcare, dontcare, fetch, mem);

	//fetch
	put_uop("regselOE, regselOESourceSel="RGS_OESRC_USEQ", regselUSeqRegSelOE="RGS_PC", memNotCS, memNotRead, irNotLoad, pcInc", next, mem);
	put_uop("", op_entry, mem);
	
	//mov's
#define MOV(o,c,z) put_op_entry("regselOE, regselOESourceSel="RGS_OESRC_OP1", regselLoad, regselLoadSourceSel="RGS_LOADSRC_OP0", aluNotALUOE, aluF="ALU_F_A, (o), (c), (z), fetch, mem)
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
	put_op_entry("pcInc, regselLoad, regselLoadSourceSel="RGS_LOADSRC_OP0", regselOE, regselOESourceSel="RGS_OESRC_USEQ", regselUSeqRegSelOE="RGS_PC", memNotCS, memNotRead", op_mov_literal, dontcare, dontcare, fetch, mem);
	
	//ALU functions
#define FN(op, f) \
	put_op_entry("regselOE, regselOESourceSel="RGS_OESRC_OP2", aluBRegNotLoad", (op), dontcare, dontcare, next, mem); \
	put_uop("regselOE, regselOESourceSel="RGS_OESRC_OP1", regselLoad, regselLoadSourceSel="RGS_LOADSRC_OP0", aluNotALUOE, statusNotLoad, aluF="  f ", aluCSel="ALU_CSEL_UCIN, fetch, mem)
	FN(op_add, ALU_F_ADD);
	FN(op_or, ALU_F_OR);
	FN(op_xor, ALU_F_XOR);
	FN(op_and, ALU_F_AND);
	/*sub*/put_op_entry("regselOE, regselOESourceSel="RGS_OESRC_OP2", aluBRegNotLoad", op_sub, dontcare, dontcare, next, mem); \
	put_uop("regselOE, regselOESourceSel="RGS_OESRC_OP1", regselLoad, regselLoadSourceSel="RGS_LOADSRC_OP0", statusNotLoad, aluNotALUOE, aluF="ALU_F_SUB", aluCSel="ALU_CSEL_UCIN", aluUCIn", fetch, mem);
	
	//shift left
	put_op_entry("regselOE, regselOESourceSel="RGS_OESRC_OP1", regselLoad, regselLoadSourceSel="RGS_LOADSRC_OP0", aluNotShiftOE, statusNotLoad, aluF="ALU_F_SHIFT_LEFT, op_shift_left, dontcare, dontcare, fetch, mem);
	
	//shift right
	put_op_entry("regselOE, regselOESourceSel="RGS_OESRC_OP1", regselLoad, regselLoadSourceSel="RGS_LOADSRC_OP0", aluNotShiftOE, statusNotLoad, aluF="ALU_F_SHIFT_RIGHT, op_shift_right, dontcare, dontcare, fetch, mem);
	
	
	//load
	put_op_entry("regselOE, regselOESourceSel="RGS_OESRC_OP1", regselLoad, regselLoadSourceSel="RGS_LOADSRC_OP0", memNotCS, memNotRead", op_load, dontcare, dontcare, fetch, mem);
	
	//store
	put_op_entry("regselOE, regselOESourceSel="RGS_OESRC_OP2", aluBRegNotLoad, aluF="ALU_F_B", aluNotALUOE", op_store, dontcare, dontcare, next, mem);
	put_uop("memNotWrite, memNotCS, regselOE, regselOESourceSel="RGS_OESRC_OP1", aluF="ALU_F_B", aluNotALUOE", next, mem);
	put_uop("memNotCS, regselOE, regselOESourceSel="RGS_OESRC_OP1", aluF="ALU_F_B", aluNotALUOE", fetch, mem);
	
	//comp
	put_op_entry("regselOE, regselOESourceSel="RGS_OESRC_OP2", aluBRegNotLoad", op_cmp, dontcare, dontcare, next, mem); \
	put_uop("regselOE, regselOESourceSel="RGS_OESRC_OP1", statusNotLoad, aluNotALUOE, aluF="ALU_F_SUB", aluCSel="ALU_CSEL_UCIN", aluUCIn", fetch, mem);
	
	//call
	put_op_entry("aluNotALUOE, regselLoad, regselLoadSourceSel="RGS_LOADSRC_USEQ", regselUSeqRegSelLoad="RGS_LR", regselOE, regselOESourceSel="RGS_OESRC_USEQ", regselUSeqRegSelOE="RGS_PC, op_call, dontcare, dontcare, next, mem);
	put_uop("aluNotALUOE, regselOE, regselOESourceSel="RGS_OESRC_OP1", regselLoad, regselLoadSourceSel="RGS_LOADSRC_USEQ", regselUSeqRegSelLoad="RGS_PC, fetch, mem);
	
	//print
	print_mem_verilog_bin(mem, mem_size, 1);
	return 0;
}
