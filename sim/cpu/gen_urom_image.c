#include <uassembler.h>

#define ADDR_WIDTH 13
#define DATA_WIDTH 48
#define OPCODE_WIDTH 7

field_description field_descrps[] = {
	{.name = "nextsel", .index = 0, .width = 1, .active = 1},
	{.name = "next", .index = 1, .width = ADDR_WIDTH, .active = 1},
	{.name = "regselOE", .index = (33 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "regselLoad", .index = (32 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "regselOESourceSel", .index = (30 + 1 + ADDR_WIDTH), .width = (31-30+1), .active = H},
	{.name = "regselLoadSourceSel", .index = (29 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "regselUSeqRegSelOE", .index = (26 + 1 + ADDR_WIDTH), .width = (28-26+1), .active = H},
	{.name = "regselUSeqRegSelLoad", .index = (23 + 1 + ADDR_WIDTH), .width = (25-23+1), .active = H},
	{.name = "pcInc", .index = (22 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "statusOE", .index = (21 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "statusNotLoad", .index = (20 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "aluBRegOE", .index = (19 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "aluBRegNotLoad", .index = (18 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "aluYRegOE", .index = (17 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "aluYRegNotLoad", .index = (16 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "aluF", .index = (11 + 1 + ADDR_WIDTH), .width = (15-11+1), .active = H},
	{.name = "aluFSel", .index = (10 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "aluCSel", .index = (9 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "aluUCIn", .index = (8 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "aluFCin", .index = (7 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "addrRegOE", .index = (6 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "addrRegNotLoad", .index = (5 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "memNotOE", .index = (4 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "memNotWE", .index = (3 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "memNotCS", .index = (2 + 1 + ADDR_WIDTH), .width = 1, .active = L},
	{.name = "irOE", .index = (1 + 1 + ADDR_WIDTH), .width = 1, .active = H},
	{.name = "irNotLoad", .index = (0 + 1 + ADDR_WIDTH), .width = 1, .active = L}
};

int main(int argc, char** argv)
{
	field_descriptions = field_descrps;
	fields_amount = sizeof(field_descrps) / sizeof(field_description);;
	addr_width = ADDR_WIDTH;
	data_width = DATA_WIDTH;
	opcode_width = OPCODE_WIDTH;
	
	init();
	fetch_index = pow(2, opcode_width + 2);
	next_uop_index = fetch_index;
	
	//ucode
	//init
	put_op_entry("", 0, dontcare, dontcare, mem);
	
	//fetch
	put_uop("regselOE, regselOESourceSel=0, regselUSeqRegSelOE=7, addrRegNotLoad, memNotCS", next, mem);
	put_uop("memNotCS, memNotOE, irNotLoad, pcInc, addrRegOE", op_entry, mem);
	
	
	//print
	print_mem_verilog_bin(mem, mem_size, 1);
	return 0;
}
