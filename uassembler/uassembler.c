#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "bin_table.h"
#include "uassembler.h"

//TODO: protection/warning against writing field second time

void print_state(uassembler* uasm, FILE* f)
{
	fprintf(f, "uassembler state:\n\tfree address: %X\n\tat addresses:\n",
			uasm->addr_free);
	int n;
	for(n = 0; n < uasm->current_addresses.len; ++n) {
		fprintf(f, "\t%d\n", uasm->current_addresses.addresses[n]);
	}
}

void check_current_addr(uassembler* uasm)
{
//	if(uasm->addr_current == -1)
//	{
//		fprintf(stderr, "error: current address is not defined");
//		print_state(uasm, stderr);
//		exit(1);
//	}
}

void assert_signal(uassembler* uasm, const char* field_name)
{
	check_current_addr(uasm);
	int field_index = bin_table_collumn_by_name(&(uasm->table), field_name);

	if(uasm->table.collumn_descriptions[field_index].width != 1)
	{
		fprintf(stderr, "error: want to assert signal with width not equal to 1\n");
		print_state(uasm, stderr);
		exit(1);
	}
	int value =  uasm->table.collumn_descriptions[field_index].active_high ? 1 : 0;
	set_field(uasm, field_name, value);

}

addresses4_t get_jmptable_addresses(uassembler* uasm, int opcode, int flags, int inspect_cpu)
{
	addresses4_t result;
	result.len = 0;
	int addr_addition = inspect_cpu ? (1 << (uasm->opcode_width + 2)) : 0;
	if(flags & NOT_CARRY_NOT_ZERO)
	{
		result.addresses[result.len++] = (opcode << 2) + addr_addition;
	}

	if(flags & NOT_CARRY_ZERO)
	{
		result.addresses[result.len++] = ((opcode << 2) | 1) + addr_addition;
	}

	if(flags & CARRY_NOT_ZERO)
	{
		result.addresses[result.len++] = ((opcode << 2) | 2) + addr_addition;
	}

	if(flags & CARRY_ZERO)
	{
		result.addresses[result.len++] = ((opcode << 2) | 3) + addr_addition;
	}

	return result;
}

void set_field(uassembler* uasm, const char* field_name, int value)
{
	check_current_addr(uasm);
	int field_index = bin_table_collumn_by_name(&(uasm->table), field_name);
	int n = 0;
	for(n = 0; n < uasm->current_addresses.len; ++n)
	{
		bin_table_set_cell_value(&(uasm->table), field_index,
				uasm->current_addresses.addresses[n], value);
	}
}

void set_default(uassembler* uasm)
{
	check_current_addr(uasm);
	int n;
	for(n = 0; n < uasm->table.collumns; n++)
	{
		if(uasm->table.collumn_descriptions[n].active_high)
		{
			set_field(uasm, uasm->table.collumn_descriptions[n].name ,0);
		}
		else
		{
			set_field(uasm, uasm->table.collumn_descriptions[n].name , (1 << uasm->table.collumn_descriptions[n].width) - 1);
		}
	}

}

void set_next_hardcoded(uassembler* uasm, int addr)
{
	set_field(uasm, uasm->nextsel_collumn_name, NEXT_SEL_UCODE);
	set_field(uasm, uasm->next_addr_collumn_name, addr);
}

void set_next(uassembler* uasm, next_sel nxt)
{
	check_current_addr(uasm);
	switch(nxt)
	{
		case next_sel_fetch:
			set_next_hardcoded(uasm, uasm->addr_fetch);
		break;
		case next_sel_next_free:
			set_next_hardcoded(uasm, uasm->addr_free);
		break;
		case next_sel_op_entry:
			set_field(uasm, uasm->nextsel_collumn_name, NEXT_SEL_OPCODE);
		break;
		case next_sel_current: {
			if(uasm->current_addresses.len > 0) {
				set_next_hardcoded(uasm, uasm->current_addresses.addresses[0]);
			}
		}
		break;
		default:
			fprintf(stderr, "error: unkown next_sel\n");
			print_state(uasm, stderr);
			exit(1);
		break;
	}
}

void goto_address(uassembler* uasm, int address)
{
	uasm->current_addresses.len = 1;
	uasm->current_addresses.addresses[0] = address;

	set_default(uasm);
}

void goto_reset(uassembler* uasm)
{
	uasm->current_at_op_entry = 0;
	goto_address(uasm, uasm->addr_reset);
}

void goto_fetch(uassembler* uasm)
{
	uasm->current_at_op_entry = 0;
	goto_address(uasm, uasm->addr_fetch);
}

int goto_next_free(uassembler* uasm)
{
	uasm->current_at_op_entry = 0;
	goto_address(uasm, uasm->addr_free);
	int result = uasm->addr_free;
	uasm->addr_free++;
	return result;
}

void goto_op_entry(uassembler* uasm, int opcode, int carry_zero_flags)
{
	if(opcode >= (1 << uasm->opcode_width))
	{
		fprintf(stderr, "error: opcode %d does not fit in opcode field\n", opcode);
		print_state(uasm, stderr);
		exit(1);
	}
	uasm->current_addresses = get_jmptable_addresses(uasm, opcode, carry_zero_flags, 0);

	set_default(uasm);
}

void goto_inspect_cpu_entry(uassembler* uasm, int opcode, int carry_zero_flags)
{
	if(opcode >= (1 << uasm->opcode_width))
	{
		fprintf(stderr, "error: opcode %d does not fit in opcode field\n", opcode);
		print_state(uasm, stderr);
		exit(1);
	}
	uasm->current_addresses = get_jmptable_addresses(uasm, opcode, carry_zero_flags, 1);

	set_default(uasm);
}


void uassembler_init(uassembler* uasm, bin_table_collumn_description* field_descriptions, int number_of_fields,
		int rom_addr_with, char* next_addr_collumn_name, char* nextsel_addr_collumn_name,
		int opcode_width, int urom_width)
{
	bin_table_new(&(uasm->table), field_descriptions, number_of_fields, 1 << rom_addr_with, urom_width, 1, 1);
	uasm->next_addr_collumn_name = next_addr_collumn_name;
	uasm->nextsel_collumn_name = nextsel_addr_collumn_name;
	uasm->opcode_width = opcode_width;
	if(rom_addr_with <= opcode_width + 3)
	{
		fprintf(stderr, "error: uassembler initialized with a too small address width");
		exit(1);
	}
	uasm->rom_addr_with = rom_addr_with;

	uasm->addr_fetch = 1 << (opcode_width + 3);
	uasm->current_addresses.len = 0;
	uasm->addr_reset = 0;
	uasm->addr_free = uasm->addr_fetch + 1;
	uasm->current_at_op_entry = 0;

}

void print_verilog(uassembler* uasm, int comments)
{
	bin_table_print_binverilog(&uasm->table, stdout, comments);
}
