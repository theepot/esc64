#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "bin_table.h"
#include "uassembler.h"

void print_state(uassembler* uasm, FILE* f)
{
	fprintf(f, "uassembler state:\n\tcurrent address: %X\n\tprevious address: %X\n\tfree address: %X\n\tat op entry: %s\n",
			uasm->addr_current, uasm->addr_previous, uasm->addr_free, uasm->current_at_op_entry ? "yes" : "no");
	if(uasm->current_at_op_entry)
	{
		fprintf(f, "\top entry carry/zero mask: %X", uasm->op_entry_zero_carry_mask);
	}
}

void check_current_addr(uassembler* uasm)
{
	if(uasm->addr_current == -1)
	{
		fprintf(stderr, "error: current address is not defined");
		print_state(uasm, stderr);
		exit(1);
	}
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

void set_field(uassembler* uasm, const char* field_name, int value)
{
	check_current_addr(uasm);
	int field_index = bin_table_collumn_by_name(&(uasm->table), field_name);
	if(uasm->current_at_op_entry)
	{
		if(uasm->op_entry_zero_carry_mask & NOT_CARRY_NOT_ZERO)
		{
			bin_table_set_cell_value(&(uasm->table), field_index,
						uasm->addr_current, value);
		}

		if(uasm->op_entry_zero_carry_mask & NOT_CARRY_ZERO)
		{
			bin_table_set_cell_value(&(uasm->table), field_index,
						uasm->addr_current | 1, value);
		}

		if(uasm->op_entry_zero_carry_mask & CARRY_NOT_ZERO)
		{
			bin_table_set_cell_value(&(uasm->table), field_index,
						uasm->addr_current | 2, value);
		}

		if(uasm->op_entry_zero_carry_mask & CARRY_ZERO)
		{
			bin_table_set_cell_value(&(uasm->table), field_index,
						uasm->addr_current | 3, value);
		}
	}
	else
	{
		bin_table_set_cell_value(&(uasm->table), field_index,
					uasm->addr_current, value);
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

void set_next(uassembler* uasm, next_sel nxt)
{
	check_current_addr(uasm);
	switch(nxt)
	{
		case next_sel_fetch:
			set_field(uasm, uasm->nextsel_collumn_name, NEXT_SEL_UCODE);
			set_field(uasm, uasm->next_addr_collumn_name, uasm->addr_fetch);
		break;
		case next_sel_next_free:
			set_field(uasm, uasm->nextsel_collumn_name, NEXT_SEL_UCODE);
			set_field(uasm, uasm->next_addr_collumn_name, uasm->addr_free);
		break;
		case next_sel_op_entry:
			set_field(uasm, uasm->nextsel_collumn_name, NEXT_SEL_OPCODE);
		break;
		case next_sel_current:
			set_field(uasm, uasm->nextsel_collumn_name, NEXT_SEL_UCODE);
			set_field(uasm, uasm->next_addr_collumn_name, uasm->addr_current);
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
	uasm->addr_previous = uasm->addr_current;
	uasm->addr_current = address;

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

void goto_next(uassembler* uasm)
{
	uasm->current_at_op_entry = 0;
	goto_address(uasm, uasm->addr_free);
	uasm->addr_free++;
}

void goto_op_entry(uassembler* uasm, int opcode, int carry_zero_mask)
{
	uasm->addr_previous = uasm->addr_current;

	if(opcode >= (1 << uasm->opcode_width))
	{
		fprintf(stderr, "error: opcode %d does not fit in opcode field\n", opcode);
		print_state(uasm, stderr);
		exit(1);
	}
	uasm->op_entry_zero_carry_mask = carry_zero_mask;
	uasm->current_at_op_entry = 1;

	goto_address(uasm, opcode << 2);
}

//TODO: is this used?
void copy_fields_from_previous(uassembler* uasm)
{
	check_current_addr(uasm);
	if(uasm->addr_previous == -1)
	{
		fprintf(stderr, "error: previous address is not defined");
		print_state(uasm, stderr);
		exit(1);
	}
	bin_table_copy_row(&uasm->table, uasm->addr_current, uasm->addr_previous);
}

void uassembler_init(uassembler* uasm, bin_table_collumn_description* field_descriptions, int number_of_fields,
		int rom_addr_with, char* next_addr_collumn_name, char* nextsel_addr_collumn_name,
		int opcode_width, int urom_width)
{
	bin_table_new(&(uasm->table), field_descriptions, number_of_fields, 1 << rom_addr_with, urom_width, 1, 1);
	uasm->next_addr_collumn_name = next_addr_collumn_name;
	uasm->nextsel_collumn_name = nextsel_addr_collumn_name;
	uasm->opcode_width = opcode_width;
	if(rom_addr_with <= opcode_width + 2)
	{
		fprintf(stderr, "error: uassembler initialized with a too small address width");
		exit(1);
	}
	uasm->rom_addr_with = rom_addr_with;

	uasm->addr_fetch = 1 << (opcode_width + 2);
	uasm->addr_current = -1;
	uasm->addr_previous = -1;
	uasm->addr_reset = 0;
	uasm->addr_free = uasm->addr_fetch + 1;
	uasm->current_at_op_entry = 0;

}

void print_verilog(uassembler* uasm, int comments)
{
	bin_table_print_binverilog(&uasm->table, stdout, comments);
}
