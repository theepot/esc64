#include <uassembler.h>

#define ADDR_WIDTH			13
#define DATA_WIDTH			(ADDR_WIDTH + 27 + 1)
#define OPCODE_WIDTH 		7

#define RGS_GP0				"0"
#define RGS_GP1				"1"
#define RGS_GP2				"2"
#define RGS_GP3				"3"
#define RGS_GP4				"4"
#define RGS_LR				"5"
#define RGS_SP				"6"
#define RGS_PC				"7"

#define RGS_LOADSRC_USEQ	"0"
#define RGS_LOADSRC_OP0 	"1"
#define RGS_OESRC_USEQ 		"0"
#define RGS_OESRC_OP0 		"1"
#define RGS_OESRC_OP1 		"2"
#define RGS_OESRC_OP2 		"3"

#define ALU_F_A				"0"
#define ALU_F_SUB			"12"
#define ALU_F_ADD			"18"
#define ALU_F_NOT			"1"
#define ALU_F_XOR			"15"
#define ALU_F_AND			"23"
#define ALU_F_OR			"29"
#define ALU_F_SHIFT_LEFT	"1"
#define ALU_F_SHIFT_RIGHT	"0"

#define ALU_FSEL_74181		"0"
#define ALU_FSEL_SHIFT		"1"

#define ALU_CSEL_UCIN		"0"
#define ALU_CSEL_FCIN		"1"

field_description field_descrps[] = {
	{.name = "nextsel", .index = 0, .width = 1, .active = 1},
	{.name = "next", .index = 1, .width = ADDR_WIDTH, .active = 1},
	{.name = "statusNotLoad", .index = (26 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "regselOE", .index = (25 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "regselLoad", .index = (24 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "regselOESourceSel", .index = (22 + 1 + ADDR_WIDTH), .width = (23-22+1), .active = H},
	{.name = "regselLoadSourceSel", .index = (21 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "regselUSeqRegSelOE", .index = (18 + 1 + ADDR_WIDTH), .width = (20-18+1), .active = H},
	{.name = "regselUSeqRegSelLoad", .index = (15 + 1 + ADDR_WIDTH), .width = (17-15+1), .active = H},
	{.name = "pcInc", .index = (14 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "aluBRegNotLoad", .index = (13 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "aluYOE", .index = (12 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "aluF", .index = (7 + 1 + ADDR_WIDTH), .width = (11-7+1), .active = H},
	{.name = "aluFSel", .index = (6 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "aluCSel", .index = (5 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "aluUCIn", .index = (4 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "memNotOE", .index = (3 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "memNotWE", .index = (2 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "memNotCS", .index = (1 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "irNotLoad", .index = (0 + 1 + ADDR_WIDTH), .width = 1, .active = L}
};

int main(int argc, char** argv)
{
	field_descriptions = field_descrps;
	fields_amount = sizeof(field_descrps) / sizeof(field_description);
	addr_width = ADDR_WIDTH;
	data_width = DATA_WIDTH;
	opcode_width = OPCODE_WIDTH;
	
	init();
	fetch_index = pow(2, opcode_width + 2);
	next_uop_index = fetch_index;
	
	//ucode
	//init
	put_op_entry("", 0, dontcare, dontcare, fetch, mem);

	//fetch
	put_uop("regselOE, regselOESourceSel="RGS_OESRC_USEQ", regselUSeqRegSelOE="RGS_PC", memNotCS, memNotOE, irNotLoad, pcInc", next, mem);
	put_uop("", op_entry, mem);
	
	//mov
	put_op_entry("regselOE, regselOESourceSel="RGS_OESRC_OP1", regselLoad, regselLoadSourceSel="RGS_LOADSRC_OP0", aluYOE, aluFSel="ALU_FSEL_74181", aluF="ALU_F_A", statusNotLoad", 1, dontcare, dontcare, fetch, mem);
	
	//mov immediate
	put_op_entry("pcInc, regselLoad, regselLoadSourceSel="RGS_LOADSRC_OP0", regselOE, regselOESourceSel="RGS_OESRC_USEQ", regselUSeqRegSelOE="RGS_PC", memNotCS, memNotOE", 2, dontcare, dontcare, fetch, mem);
	
	//add
	put_op_entry("regselOE, regselOESourceSel="RGS_OESRC_OP2", aluBRegNotLoad", 3, dontcare, dontcare, next, mem);
	put_uop("regselOE, regselOESourceSel="RGS_OESRC_OP1", regselLoad, regselLoadSourceSel="RGS_LOADSRC_OP0", aluYOE, statusNotLoad, aluFSel="ALU_FSEL_74181", aluF="ALU_F_ADD", aluCSel="ALU_CSEL_UCIN, fetch, mem);
	
	
	//print
	print_mem_verilog_bin(mem, mem_size, 1);
	return 0;
}
