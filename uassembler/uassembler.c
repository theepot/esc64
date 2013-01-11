#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "uassembler.h"

void set_uop(const char* const fieldvalues, int next, int nextsel, int uopn)
{
		change_fields_by_string(fieldvalues, uopn);
		change_field(find_field_by_name("next"), next, uopn);
		change_field(find_field_by_name("nextsel"), nextsel, uopn);
}

void put_op_entry(uassembler* uasm, const char* const fieldvalues, int opcode, flag_condition c, flag_condition z, const next_sel nxt)
{

	int nxt_addr = 0;
	int nxt_sel = NEXT_SEL_UCODE;
	switch(nxt)
	{
		case fetch: nxt_addr = uasm->fetch_index; break;
		case next: nxt_addr = uasm->next_uop_index; break;
		case op_entry: nxt_sel = NEXT_SEL_OPCODE; break;
		default:
			fprintf(stderr, "error: unkown next_sel\n");
			exit(1);
		break;
	}
	//notzero notcarry
	if((c == dontcare || c == false) && (z == dontcare || z == false))
		set_uop(fieldvalues, nxt_addr, nxt_sel, opcode << 2);
	
	//notzero carry
	if((c == dontcare || c == true) && (z == dontcare || z == false))
		set_uop(fieldvalues, nxt_addr, nxt_sel, (opcode << 2) | 2);

	//zero notcarry
	if((c == dontcare || c == false) && (z == dontcare || z == true))
		set_uop(fieldvalues, nxt_addr, nxt_sel, (opcode << 2) | 1);
	
	//zero carry
	if((c == dontcare || c == true) && (z == dontcare || z == true))
		set_uop(fieldvalues, nxt_addr, nxt_sel, (opcode << 2) | 3);
	
}

void put_uop(uassembler* uasm, const char* const fieldvalues, const next_sel nxt)
{
	switch(nxt)
	{
		case fetch:
			set_uop(uasm, fieldvalues, uasm->fetch_index, NEXT_SEL_UCODE, uasm->next_uop_index);
		break;
		case next:
			set_uop(uasm, fieldvalues, uasm->next_uop_index + 1, NEXT_SEL_UCODE,  uasm->next_uop_index);
		break;
		case op_entry:
			set_uop(uasm, fieldvalues, 0, NEXT_SEL_OPCODE, uasm->next_uop_index);
		break;
		default:
			fprintf(stderr, "error: unkown next_sel\n");
			exit(1);
		break;
	}
	
	uasm->next_uop_index++;
}

void uassembler_init(uassembler* uasm, bin_table_collumn_description* field_descriptions, int number_of_fields,
		int rom_addr_with, char* next_addr_collumn_name, char* nextsel_addr_collumn_name,
		int opcode_width)
{
	bin_table_new(uasm->table, field_descriptions, number_of_fields, 1 << rom_addr_with , 1, 1);
	uasm->next_addr_collumn_name = next_addr_collumn_name;
	uasm->nextsel_addr_collumn_name = nextsel_addr_collumn_name;
	uasm->opcode_width = opcode_width;
	if(rom_addr_with <= opcode_width + 2)
	{
		fprintf(stderr, "error: uassembler initialized with a too small address with");
		exit(1);
	}
	uasm->fetch_index = 1 << (opcode_width + 2);
	uasm->rom_addr_with = rom_addr_with;
}
