#include "mini_assembler.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cpu.h>
#include <instr_info.h>

struct globals_struct globals;

int label_find_by_name(const char* const name)
{
	int i;
	for(i = 0; i < globals.labels_amount; i++)
	{
		if(!strcmp(name, globals.labels[i].name))
			return i;
	}
	
	return -1;
}

int label_find_by_value(const int value)
{
	int i;
	for(i = 0; i < globals.labels_amount; i++)
	{
		if(globals.labels[i].value == value)
			return i;
	}
	
	return -1;
}

void init_mem()
{
	int i;
	for(i = 0; i < globals.mem_size; i++)
		globals.mem[i].comment = 0;
		globals.mem[i].valid = 0;
		globals.mem[i].is_literal = 0;
}

void print_binary(const int n, const int bits)
{
	int i;
	for(i = 0; i < bits; i++)
	{
		putchar('0' + ((n >> (bits - i - 1)) & 1));
	}

}

void print_memory()
{
	int i;
	for(i = 0; i < globals.mem_size; i++)
	{
		int lbln = label_find_by_value(i);
		if(lbln != -1)
		{
			printf("// %s\n", globals.labels[lbln].name);
		
		}
		if(globals.mem[i].valid)
		{
			if(globals.mem[i].is_literal)
			{
				int l;
				if(globals.mem[i].is_reference)
				{
					l = globals.labels[globals.mem[i].literal].value;
					if(l == -1)
					{
						fprintf(stderr, "ERROR: found label with undefined value. Label name: %s\n", globals.labels[globals.mem[i].literal].name);
					}
				}
				else
				{
					l = globals.mem[i].literal;
				}
				print_binary(l, 8);
				putchar('\n');
				print_binary((l >> 8), 8);

			}
			else
			{
				uint16_t tmp = (globals.mem[i].opcode << 9) | (globals.mem[i].op0 << 6) | (globals.mem[i].op1 << 3) | globals.mem[i].op2;
				print_binary(tmp, 8);
				putchar('\n');
				print_binary((tmp >> 8), 8);
			}
		}
		else
		{
			printf("xxxxxxxx\nxxxxxxxx");
		}
		if(globals.mem[i].comment)
			printf(" // %X %s\n", i * 2, globals.mem[i].comment);
		else
			printf(" // %X\n", i * 2);
	}
}


void add_instruction(const opcode_t opcode, const int op0, const int op1, const int op2, const char* const comment)
{
	if(globals.new_instruction >= globals.mem_size)
	{
		fprintf(stderr, "ERROR: no more space for instructions. Quiting");
		exit(1);
	}
	globals.mem[globals.new_instruction].valid = 1;
	globals.mem[globals.new_instruction].opcode = opcode;
	globals.mem[globals.new_instruction].op0 = op0;
	globals.mem[globals.new_instruction].op1 = op1;
	globals.mem[globals.new_instruction].op2 = op2;
	globals.mem[globals.new_instruction].comment = comment;
	globals.new_instruction++;
}

void add_literal(const int value, const char* const comment)
{
	if(globals.new_instruction >= globals.mem_size)
	{
		fprintf(stderr, "ERROR: no more space for instructions. Quiting");
		exit(1);
	}
	if(value >= (1<<16))
	{
		fprintf(stderr, "ERROR: literal value to big to fit in 16 bit. Quiting\n");
		exit(1);
	}
	globals.mem[globals.new_instruction].valid = 1;
	globals.mem[globals.new_instruction].is_literal = 1;
	globals.mem[globals.new_instruction].literal = value;
	globals.mem[globals.new_instruction].comment = comment;
	globals.new_instruction++;
}

int get_label(const char* const name)
{
	int label_id = label_find_by_name(name);
	if(label_id != -1)
	{
		return label_id;
	}
	if(globals.labels_amount >= LABELS_MAX)
	{
		fprintf(stderr, "ERROR: no more space for new labels. Quiting");
		exit(1);
	}
	label_id = globals.labels_amount;
	globals.labels[label_id].name = name;
	globals.labels[label_id].value = -1;
	globals.labels_amount++;
	return label_id;
}

void add_literal_labeled(const char* name, const char* const comment)
{
	if(globals.new_instruction >= globals.mem_size)
	{
		fprintf(stderr, "ERROR: no more space for instructions. Quiting");
		exit(1);
	}
	globals.mem[globals.new_instruction].valid = 1;
	globals.mem[globals.new_instruction].is_literal = 1;
	globals.mem[globals.new_instruction].is_reference = 1;
	globals.mem[globals.new_instruction].literal = get_label(name);

	globals.mem[globals.new_instruction].comment = comment;
	globals.new_instruction++;
}

void set_label(const int id, const int value)
{
	if(value >= (1<<16))
	{
		fprintf(stderr, "ERROR: literal value to big to fit in 16 bit. Quiting\n");
		exit(1);
	}
	globals.labels[id].value = value;
}

void lbl(const char* const name)
{
	int id = get_label(name);
	if(globals.labels[id].value == -1)
	{
		set_label(id, globals.new_instruction * 2);
	}
	else
	{
		fprintf(stderr, "ERROR: label %s declared second time\n", name);
	}
}

void org(int loc)
{
	globals.new_instruction = loc;
}

//operations
void mov(const int dst, const int src)
{
	add_instruction(op_mov, dst, src, 0, "move");
}

void mov_on_notcarry_and_notzero(const int dst, const int src)
{
	add_instruction(op_movncnz, dst, src, 0, "mov_on_notcarry_and_notzero");
}

void mov_on_notcarry_and_zero(const int dst, const int src)
{
	add_instruction(op_movncz, dst, src, 0, "mov_on_notcarry_and_zero");
}

void mov_on_notcarry(const int dst, const int src)
{
	add_instruction(op_movnc, dst, src, 0, "mov_on_notcarry");
}

void mov_on_carry_and_notzero(const int dst, const int src)
{
	add_instruction(op_movcnz, dst, src, 0, "mov_on_carry_and_notzero");
}

void mov_on_notzero(const int dst, const int src)
{
	add_instruction(op_movnz, dst, src, 0, "mov_on_notzero");
}

void mov_on_notcarry_or_notzero(const int dst, const int src)
{
	add_instruction(op_movncnz, dst, src, 0, "mov_on_notcarry_or_notzero");
}

void mov_on_carry_and_zero(const int dst, const int src)
{
	add_instruction(op_movcz, dst, src, 0, "mov_on_carry_and_zero");
}

void mov_on_zero(const int dst, const int src)
{
	add_instruction(op_movz, dst, src, 0, "mov_on_zero");
}

void mov_on_notcarry_or_zero(const int dst, const int src)
{
	add_instruction(op_movncoz, dst, src, 0, "mov_on_notcarry_or_zero");
}

void mov_on_carry(const int dst, const int src)
{
	add_instruction(op_movc, dst, src, 0, "mov_on_carry");
}

void mov_on_carry_or_notzero(const int dst, const int src)
{
	add_instruction(op_movconz, dst, src, 0, "mov_on_carry_or_notzero");
}

void mov_on_carry_or_zero(const int dst, const int src)
{
	add_instruction(op_movcoz, dst, src, 0, "mov_on_carry_or_zero");
}

void mov_literal(const int dst, const int literal)
{
	add_instruction(op_mov_imm, dst, 0, 0, "move_literal");
	add_literal(literal, 0);
}

void mov_literal_on_notcarry_and_notzero(const int dst, const int literal)
{
	add_instruction(op_movncnz_imm, dst, 0, 0, "mov_literal_on_notcarry_and_notzero");
	add_literal(literal, 0);
}

void mov_literal_on_notcarry_and_zero(const int dst, const int literal)
{
	add_instruction(op_movncz_imm, dst, 0, 0, "mov_literal_on_notcarry_and_zero");
	add_literal(literal, 0);
}

void mov_literal_on_notcarry(const int dst, const int literal)
{
	add_instruction(op_movnc_imm, dst, 0, 0, "mov_literal_on_notcarry");
	add_literal(literal, 0);
}

void mov_literal_on_carry_and_notzero(const int dst, const int literal)
{
	add_instruction(op_movcnz_imm, dst, 0, 0, "mov_literal_on_carry_and_notzero");
	add_literal(literal, 0);
}

void mov_literal_on_notzero(const int dst, const int literal)
{
	add_instruction(op_movnz_imm, dst, 0, 0, "mov_literal_on_notzero");
	add_literal(literal, 0);
}

void mov_literal_on_notcarry_or_notzero(const int dst, const int literal)
{
	add_instruction(op_movnconz_imm, dst, 0, 0, "mov_literal_on_notcarry_or_notzero");
	add_literal(literal, 0);
}

void mov_literal_on_carry_and_zero(const int dst, const int literal)
{
	add_instruction(op_movcz_imm, dst, 0, 0, "mov_literal_on_carry_and_zero");
	add_literal(literal, 0);
}

void mov_literal_on_zero(const int dst, const int literal)
{
	add_instruction(op_movz_imm, dst, 0, 0, "mov_literal_on_zero");
	add_literal(literal, 0);
}

void mov_literal_on_notcarry_or_zero(const int dst, const int literal)
{
	add_instruction(op_movncoz_imm, dst, 0, 0, "mov_literal_on_notcarry_or_zero");
	add_literal(literal, 0);
}

void mov_literal_on_carry(const int dst, const int literal)
{
	add_instruction(op_movc, dst, 0, 0, "mov_literal_on_carry");
	add_literal(literal, 0);
}

void mov_literal_on_carry_or_notzero(const int dst, const int literal)
{
	add_instruction(op_movconz_imm, dst, 0, 0, "mov_literal_on_carry_or_notzero");
	add_literal(literal, 0);
}

void mov_literal_on_carry_or_zero(const int dst, const int literal)
{
	add_instruction(op_movcoz_imm, dst, 0, 0, "mov_literal_on_carry_or_zero");
	add_literal(literal, 0);
}

void mov_literal_labeled(const int dst, const char* const name)
{
	add_instruction(op_mov_imm, dst, 0, 0, "move_literal labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_notcarry_and_notzero_labeled(const int dst, const char* const name)
{
	add_instruction(op_movncnz_imm, dst, 0, 0, "mov_literal_on_notcarry_and_notzero labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_notcarry_and_zero_labeled(const int dst, const char* const name)
{
	add_instruction(op_movncz_imm, dst, 0, 0, "mov_literal_on_notcarry_and_zero labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_notcarry_labeled(const int dst, const char* const name)
{
	add_instruction(op_movnc_imm, dst, 0, 0, "mov_literal_on_notcarry labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_carry_and_notzero_labeled(const int dst, const char* const name)
{
	add_instruction(op_movcnz_imm, dst, 0, 0, "mov_literal_on_carry_and_notzero labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_notzero_labeled(const int dst, const char* const name)
{
	add_instruction(op_movnz_imm, dst, 0, 0, "mov_literal_on_notzero labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_notcarry_or_notzero_labeled(const int dst, const char* const name)
{
	add_instruction(op_movnconz_imm, dst, 0, 0, "mov_literal_on_notcarry_or_notzero labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_carry_and_zero_labeled(const int dst, const char* const name)
{
	add_instruction(op_movcz_imm, dst, 0, 0, "mov_literal_on_carry_and_zero labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_zero_labeled(const int dst, const char* const name)
{
	add_instruction(op_movz_imm, dst, 0, 0, "mov_literal_on_zero labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_notcarry_or_zero_labeled(const int dst, const char* const name)
{
	add_instruction(op_movncoz_imm, dst, 0, 0, "mov_literal_on_notcarry_or_zero labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_carry_labeled(const int dst, const char* const name)
{
	add_instruction(op_movc_imm, dst, 0, 0, "mov_literal_on_carry labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_carry_or_notzero_labeled(const int dst, const char* const name)
{
	add_instruction(op_movconz_imm, dst, 0, 0, "mov_literal_on_carry_or_notzero labeled");
	add_literal_labeled(name, name);
}

void mov_literal_on_carry_or_zero_labeled(const int dst, const char* const name)
{
	add_instruction(op_movcoz_imm, dst, 0, 0, "mov_literal_on_carry_or_zero labeled");
	add_literal_labeled(name, name);
}

void add(const int dst, const int a, const int b)
{
	add_instruction(op_add, dst, a, b, "add");
}

void add_with_carry(const int dst, const int a, const int b)
{
	add_instruction(op_adc, dst, a, b, "add with carry");
}

void sub(const int dst, const int a, const int b)
{
	add_instruction(op_sub, dst, a, b, "sub");
}

void sub_with_carry(const int dst, const int a, const int b)
{
	add_instruction(op_sbc, dst, a, b, "sub with carry");
}

void and(const int dst, const int a, const int b)
{
	add_instruction(op_and, dst, a, b, "and");
}

void or(const int dst, const int a, const int b)
{
	add_instruction(op_or, dst, a, b, "or");
}

void xor(const int dst, const int a, const int b)
{
	add_instruction(op_xor, dst, a, b, "xor");
}

void not(const int dst, const int a)
{
	add_instruction(op_not, dst, a, 0, "not");
}

void inc(const int dst, const int a)
{
	add_instruction(op_inc, dst, a, 0, "inc");
}

void dec(const int dst, const int a)
{
	add_instruction(op_dec, dst, a, 0, "dec");
}

void shiftl(const int dst, const int src)
{
	add_instruction(op_shl, dst, src, 0, "shiftl");
}

void shiftr(const int dst, const int src)
{
	add_instruction(op_shr, dst, src, 0, "shiftr");
}

void load(const int dst, const int src)
{
	add_instruction(op_ldr, dst, src, 0, "load");
}

void store(const int dst, const int src)
{
	add_instruction(op_str, 0, dst, src, "store");
}

void call(const int addr_reg)
{
	add_instruction(op_call, 0, addr_reg, 0, "call");
}

void call_literal(const int addr)
{
	add_instruction(op_call_imm, 0, 0, 0, "call_literal");
	add_literal(addr, 0);
}
void call_literal_labeled(const char* const name)
{
	add_instruction(op_call_imm, 0, 0, 0, "call_literal");
	add_literal_labeled(name, name);
}

void in(const int dst, const int src)
{
	add_instruction(op_in, dst, src, 0, "in");
}

void out(const int dst, const int src)
{
	add_instruction(op_out, 0, dst, src, "store");
}

void cmp(const int a, const int b)
{
	add_instruction(op_cmp, 0, a, b, "compare");
}

void halt()
{
	add_instruction(op_halt, 0, 0, 0, "halt");
}

void push(int a)
{
	add_instruction(op_push, 0, a, 0, "push");
}

void pop(int a)
{
	add_instruction(op_pop, a, 0, 0, "pop");
}

//pseudo operations
void jump(const char* const name)
{
	mov_literal_labeled(PC, name);
}



void save_all(void)
{
	push(GP0);
	push(GP1);
	push(GP2);
	push(GP3);
	push(GP4);
}

void restore_all(void)
{
	pop(GP4);
	pop(GP3);
	pop(GP2);
	pop(GP1);
	pop(GP0);
}

extern void asm_prgm(void);

int main(int argc, char** argv)
{
	//init
	globals.mem_size = MEM_SIZE;
	globals.mem = malloc(sizeof(instruction) * globals.mem_size);
	globals.new_instruction = 0;
	globals.labels = malloc(sizeof(label) * LABELS_MAX);
	globals.labels_amount = 0;
	init_mem();


	asm_prgm();

	//end program
	print_memory();
	return 0;
}
