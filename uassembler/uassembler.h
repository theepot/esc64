#ifndef __UASSEMBLER_H
#define __UASSEMBLER_H

#define L 0
#define H 1

#define NEXT_SEL_UCODE 0
#define NEXT_SEL_OPCODE 1

typedef struct {
	field_description* field_descriptions;
	int fields_amount = 0;
	int addr_width = 0;
	int data_width = 0;
	int opcode_width = 0;
	char* mem = 0;
	int mem_size = 0;
	int fetch_index = 0;
	int next_uop_index = 0;

} uassembler;

typedef struct {
	char* name;
	int index;
	int width;
	int active;
} field_description;

typedef enum next_sel{
	fetch,
	next,
	op_entry
} next_sel;

typedef enum flag_condition{
	false,
	true,
	dontcare
} flag_condition;


int compare_field_descriptions(const void* a, const void* b);
void initialise_field_descriptions();
int find_field_by_name(const char* const name);
void print_mem_verilog_bin(const char* const m, const int size, const int comments);
void change_field(const int field_number, const int value, char* const mem, const int uopn);
void turn_all_fields_inactive(char* const mem, const int uopn);
void change_fields_by_string(const char* const fieldvalues, char* const mem, const int uopn);
void set_uop(const char* const fieldvalues, int next, int nextsel, char* const mem, int uopn);
void put_op_entry(const char* const fieldvalues, int opcode, flag_condition c, flag_condition z, const next_sel nxt, char* mem)
void put_uop(const char* const fieldvalues, const next_sel nxt, char* mem);
void init();

#endif
