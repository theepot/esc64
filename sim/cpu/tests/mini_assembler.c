#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cpu.h"

#define MEM_SIZE (2048)
#define LABELS_MAX (256)

#define GP0	0
#define GP1	1
#define GP2	2
#define GP3 3
#define GP4	4
#define GP5	5
#define LR	6
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

void mov_on_zero(const int dst, const int src)
{
	add_instruction(op_mov_on_zero, dst, src, 0, "move on zero");
}
void mov_on_equal(const int dst, const int src)
{
	mov_on_zero(dst, src);
}

void mov_on_not_zero(const int dst, const int src)
{
	add_instruction(op_mov_on_not_zero, dst, src, 0, "move on not zero");
}
void mov_on_not_equal(const int dst, const int src)
{
	mov_on_not_zero(dst, src);
}

void mov_on_not_carry(const int dst, const int src)
{
	add_instruction(op_mov_on_not_carry, dst, src, 0, "move on not carry");
}
void mov_on_less_than(const int dst, const int src)
{
	mov_on_not_carry(dst, src);
}

void mov_on_not_carry_or_zero(const int dst, const int src)
{
	add_instruction(op_mov_on_not_carry_or_zero, dst, src, 0, "move on not carry or zero");
}
void mov_on_less_than_or_equal(const int dst, const int src)
{
	mov_on_not_carry_or_zero(dst, src);
}


void mov_literal(const int dst, const int literal)
{
	add_instruction(op_mov_literal, dst, 0, 0, "move literal");
	add_literal(literal, 0);
}

void mov_literal_labeled(const int dst, const char* const name)
{
	add_instruction(op_mov_literal, dst, 0, 0, "move literal labeled");
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

void shiftl(const int dst, const int src)
{
	add_instruction(op_shift_left, dst, src, 0, "shiftl");
}

void shiftr(const int dst, const int src)
{
	add_instruction(op_shift_right, dst, src, 0, "shiftr");
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
	unsigned dest = 0xFFFF;
	
#define FN3(f, a, b, e, z, c) \
	dest -= 3; \
	fprintf(stderr, "@%05u expect: %04X, %c, %c\n", dest, (e), (z) ? '1' : '0', (c) ? '1' : '0'); \
	mov_literal(GP0, (a)); \
	mov_literal(GP1, (b)); \
	mov_literal(GP2, dest); \
	f(GP0, GP0, GP1); \
	store(GP2, GP0); \
	mov_literal(GP0, 1); \
	mov(GP1, GP0); \
	mov_literal(GP2, dest+1); \
	mov_literal(GP3, dest+2); \
	mov_literal(GP4, 0); \
	mov_on_not_zero(GP0, GP4); \
	mov_on_not_carry(GP1, GP4); \
	store(GP2, GP0); \
	store(GP3, GP1);
	
#define FN2(f, a, e, z, c) \
	dest -= 3; \
	fprintf(stderr, "@%05u expect: %04X, %c, %c\n", dest, (e), (z) ? '1' : '0', (c) ? '1' : '0'); \
	mov_literal(GP1, (a)); \
	mov_literal(GP2, dest); \
	f(GP0, GP1); \
	store(GP2, GP0); \
	mov_literal(GP0, 1); \
	mov(GP1, GP0); \
	mov_literal(GP2, dest+1); \
	mov_literal(GP3, dest+2); \
	mov_literal(GP4, 0); \
	mov_on_not_zero(GP0, GP4); \
	mov_on_not_carry(GP1, GP4); \
	store(GP2, GP0); \
	store(GP3, GP1);

#define ADD(a, b, e, z, c) FN3(add, a, b, e, z, c)
#define SUB(a, b, e, z, c) FN3(sub, a, b, e, z, c)
#define OR(a, b, e, z, c) FN3(or, a, b, e, z, c)
#define XOR(a, b, e, z, c) FN3(xor, a, b, e, z, c)
#define AND(a, b, e, z, c) FN3(and, a, b, e, z, c)
#define SHIFTL(a, e, z, c) FN2(shiftl, a, e, z, c)
#define SHIFTR(a, e, z, c) FN2(shiftr, a, e, z, c)
	
	ADD(2, 1, 3, 0, 0);
	ADD(0xFFFF, 1, 0, 1, 1);
	ADD(0xFFFF, 2, 1, 0, 1);
	
	SUB(0xFF, 0xF, 0xFF-0xF, 0, 1);
	SUB(1, 10, 0xFFF7, 0, 0);
	SUB(7, 7, 7-7, 1, 1);
	
	OR(123, 0, 123, 0, 0);
	OR(0, 0, 0, 1, 0);
	
	XOR(0xAAAA, 0xAAAA, 0, 1, 0);
	XOR(0xAAAA, 0xBBBB, 0xAAAA^0xBBBB, 0, 0);
	
	AND(0xFC, 0x3F, 0xFC&0x3F, 0, 0);
	AND(0xF0F0, 0, 0, 1, 0);
	
	SHIFTL(0x0001, 0x0002, 0, 0);
	SHIFTL(0x8001, 0x0002, 0, 1);
	SHIFTL(0x0000, 0x0000, 1, 0);
	SHIFTL(0x8000, 0x0000, 1, 1);
	
	SHIFTR(0x1000, 0x0800, 0, 0);
	SHIFTR(0x1001, 0x0800, 0, 1);
	SHIFTR(0x0000, 0x0000, 1, 0);
	SHIFTR(0x0001, 0x0000, 1, 1);
	
	
	halt();
	
	/*mov_literal(0, 10);
	mov_literal(1, 5);
	mov_literal_labeled(2, "sub0");
	call(2);
	call(2);
	halt();
	
	lbl("sub0"); //start sub0
	add(0, 0, 1);
	mov(PC, LR); //return*/
	
		/*mov_literal(0, 10);	//r0 = 10
		mov_literal(1, 1);	//r1 = 1
		mov_literal(5, 1);	//r5 = 0
		mov_literal_labeled(2, "end0");	//r2 = end0
		
	lbl("loop0");
		sub(0, 0, 1);
		mov_on_equal(PC, 2);
		
		sub(GP5, GP5, 1);
		store(GP5, 0);
		
		jump("loop0");
	lbl("end0");
	
		mov_literal(0, 10);	//r0 = 10
		mov_literal_labeled(2, "end1");	//r2 = end1
	
	lbl("loop1");
		sub(0, 0, 1);
		mov_on_equal(PC, 2);
		
		load(3, GP5);
		add(GP5, GP5, 1);
		
		jump("loop1");
	lbl("end1");
	
		halt();*/
	
	/*mov_literal(0, 10);
	mov_literal(1, 10);
	mov_literal_labeled(2, "end");
	cmp(0, 1);
	//mov_on_less_than_or_equal(PC, 2);
	mov_on_not_equal(PC, 2);
	add(0, 0, 1);
	lbl("end");
	halt();*/
	
	
	/*mov_literal(1, 4);
	mov_literal_labeled(0, "fib");
	call(0);
	halt();

	//0 = call target
	//1 = n
	//2 = 1
	//3 = 2
lbl("fib");
	//constants
	mov(2, 1);
	mov(3, 2);

lbl("fib_r");
	//if n < 2 then return n
	mov_literal_labeled(0, "retN");	//jump target for then
	cmp(1, 2);		//if n <= 1
	mov_on_less_than_or_equal(PC, 0);			//then goto retN
	
	
	//a = fib(n - 1)
	sub(SP, SP, 2);	//push return address
	store(SP, LR);
	sub(LR, LR, 2);	//push n
	store(SP, 1);
	
	mov_literal_labeled(0, "fib_r");	//fib_r, call target
	sub(1, 1, 2);	//n = n - 1
	call(0);			//call fib(n - 1)
	
	sub(SP, SP, 2);	//push a
	store(SP, 0);
	
	//b = fib(n - 2)
	add(SP, SP, 2);	//peek n
	load(1, SP);
	sub(SP, SP, 2);
	
	sub(1, 1, 3);	//n = n - 2
	
	mov_literal_labeled(0, "fib_r");	//fib_r, call target
	call(0);		//call fib(n - 2)
	
	load(1, SP);		//pop a from stack
	add(SP, SP, 2);
	
	add(1, 0, 1);	//n = a + b
	
	//cleanup	
	add(SP, SP, 2);	//pop n
	mov(LR, SP);		//pop return address
	add(SP, SP, 2);
	
	//fallthrough
lbl("retN");
	mov(0, 1);
	mov(PC, LR);*/
	
	
	
	//end program
	print_memory();
	return 0;
}
