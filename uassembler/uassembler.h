#ifndef __UASSEMBLER_H
#define __UASSEMBLER_H

#include "bin_table.h"
#include <stdio.h>

#define L 0
#define H 1

#define NEXT_SEL_UCODE 0
#define NEXT_SEL_OPCODE 1

typedef struct
{
	bin_table* table;
	char* next_addr_collumn_name;
	char* nextsel_addr_collumn_name;
	int rom_addr_with;
	int opcode_width;
	int fetch_index;
	int next_uop_index;
} uassembler;

typedef struct
{
	char* name;
	int index;
	int width;
	int active;
} field_description;

typedef enum next_sel
{
	fetch,
	next,
	op_entry
} next_sel;

typedef enum flag_condition
{
	false,
	true,
	dontcare
} flag_condition;


void set_uop(const char* const fieldvalues, int next, int nextsel, char* const mem, int uopn);
void put_op_entry(const char* const fieldvalues, int opcode, flag_condition c, flag_condition z, const next_sel nxt, char* mem)
void put_uop(const char* const fieldvalues, const next_sel nxt, char* mem);
void uassembler_init(uassembler* uasm, bin_table_collumn_description* field_descriptions, int number_of_fields,
		int rom_addr_with, char* next_addr_collumn_name, char* nextsel_addr_collumn_name,
		int opcode_width);
void fprint_urom(FILE* f, uassembler* uasm);

#endif
