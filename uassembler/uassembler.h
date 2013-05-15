#ifndef __UASSEMBLER_H
#define __UASSEMBLER_H
#include "bin_table.h"
#include <stdio.h>

#define L						0
#define H						1

#define NEXT_SEL_UCODE			0
#define NEXT_SEL_OPCODE			1

#define NOT_CARRY_NOT_ZERO		(1 << 0)
#define NOT_CARRY_ZERO			(1 << 1)
#define CARRY_NOT_ZERO			(1 << 2)
#define CARRY_ZERO				(1 << 3)

#define ALWAYS					(NOT_CARRY_NOT_ZERO | NOT_CARRY_ZERO | CARRY_NOT_ZERO | CARRY_ZERO)

typedef struct
{
	bin_table table;

	char* next_addr_collumn_name;
	char* nextsel_collumn_name;

	int rom_addr_with;
	int opcode_width;

	int addr_fetch;
	int addr_current;
	int addr_free;
	int addr_reset;
	int addr_previous;

	int current_at_op_entry;
	int op_entry_zero_carry_mask;
} uassembler;

typedef enum next_sel
{
	next_sel_fetch,
	next_sel_next_free,
	next_sel_op_entry,
	next_sel_current
} next_sel;

typedef enum flag_condition
{
	false,
	true,
	dontcare
} flag_condition;

void uassembler_init(uassembler* uasm, bin_table_collumn_description* field_descriptions, int number_of_fields,
		int rom_addr_with, char* next_addr_collumn_name, char* nextsel_addr_collumn_name,
		int opcode_width, int urom_width);
void fprint_urom(FILE* f, uassembler* uasm);
void assert_signal(uassembler* uasm, const char* field_name);
void set_field(uassembler* uasm, const char* field_name, int value);
void set_default(uassembler* uasm);
void set_next(uassembler* uasm, next_sel nxt);
void goto_address(uassembler* uasm, int address);
void goto_reset(uassembler* uasm);
void goto_fetch(uassembler* uasm);
void goto_next_free(uassembler* uasm);
void goto_op_entry(uassembler* uasm, int opcode, int carry_zero_mask);
void copy_fields_from_previous(uassembler* uasm); //TODO: remove. this is deprecated
void check_current_addr(uassembler* uasm);
void print_state(uassembler* uasm, FILE* f);
void print_verilog(uassembler* uasm, int comments);

#endif
