#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cpu.h"

#define MEM_SIZE (64)
#define LABELS_MAX (256)

#define GP0	0
#define GP1	1
#define GP2	2
#define GP3 3
#define GP4	4
#define LR	5
#define SP	6
#define PC	7

typedef struct {
	int opcode;
	int op0, op1, op2;
	int valid;
	const char* comment;
	int is_literal;
	int literal; //or label id when is_reference is true
	int is_reference;
} instruction;

typedef struct {
	const char* name;
	int value;
} label;

struct {
	int new_instruction;
	instruction* mem;
	int mem_size;
	label* labels;
	int labels_amount;
} globals;

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
				print_binary(l, 16);
				printf("   ");
			}
			else
			{
				print_binary(globals.mem[i].opcode, OPCODE_WIDTH);
				putchar('_');
				print_binary(globals.mem[i].op0, 3);
				putchar('_');
				print_binary(globals.mem[i].op1, 3);
				putchar('_');
				print_binary(globals.mem[i].op2, 3);
			}
		}
		else
		{
			int tmp;
			for(tmp = 0; tmp < OPCODE_WIDTH; tmp++)
				putchar('x');
			printf("_xxx_xxx_xxx");
		}
		if(globals.mem[i].comment)
			printf(" // %X %s\n", i, globals.mem[i].comment);
		else
			printf(" // %X\n", i);
	}
}


void add_instruction(const int opcode, const int op0, const int op1, const int op2, const char* const comment)
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
		set_label(id, globals.new_instruction);
	}
	else
	{
		fprintf(stderr, "ERROR: label %s declared second time\n", name);
	}
}

//operations
void mov(const int dst, const int src)
{
	add_instruction(op_mov, dst, src, 0, "move");
}

void mov_on_equal(const int dst, const int src)
{
	add_instruction(op_mov_on_equal, dst, src, 0, "move on equal");
}

void mov_on_not_equal(const int dst, const int src)
{
	add_instruction(op_mov_on_not_equal, dst, src, 0, "move on not equal");
}

void mov_on_less_than(const int dst, const int src)
{
	add_instruction(op_mov_on_less_than, dst, src, 0, "move on less than");
}

void mov_on_less_than_or_equal(const int dst, const int src)
{
	add_instruction(op_mov_on_less_than_or_equal, dst, src, 0, "move on less than or equal");
}

void mov_literal(const int dst, const int literal)
{
	add_instruction(op_mov_big_literal, dst, 0, 0, "move literal");
	add_literal(literal, 0);
}

void mov_literal_labeled(const int dst, const char* const name)
{
	add_instruction(op_mov_big_literal, dst, 0, 0, "move literal labeled");
	add_literal_labeled(name, 0);
}

void add(const int dst, const int a, const int b)
{
	add_instruction(op_add, dst, a, b, "add");
}

void sub(const int dst, const int a, const int b)
{
	add_instruction(op_sub, dst, a, b, "sub");
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

void load(const int dst, const int src)
{
	add_instruction(op_load, dst, src, 0, "load");
}

void store(const int dst, const int src)
{
	add_instruction(op_store, 0, dst, src, "store");
}

void call(const int addr_reg)
{
	add_instruction(op_call, 0, addr_reg, 0, "call");
}

void cmp(const int a, const int b)
{
	add_instruction(op_cmp, 0, a, b, "compare");
}

//pseudo operations
void jump(const char* const name)
{
	mov_literal_labeled(PC, name);
}

void halt()
{
	add_instruction(0x7F, 0, 0, 0, "halt");
}

int main(int argc, char** argv)
{
	//init
	globals.mem_size = MEM_SIZE;
	globals.mem = malloc(sizeof(instruction) * globals.mem_size);
	globals.new_instruction = 0;
	globals.labels = malloc(sizeof(label) * LABELS_MAX);
	globals.labels_amount = 0;
	init_mem();
	
	//begin program
	mov_literal(0, 10);
	mov_literal(1, 1);
	mov_literal_labeled(2, "end");
	lbl("loop0");
	sub(0, 0, 1);
	mov_on_equal(PC, 2);
	jump("loop0");
	lbl("end");
	halt();
	
	//end program
	print_memory();
	return 0;
}
