#ifndef MINI_ASSEMBLER_INCLUDED
#define MINI_ASSEMBLER_INCLUDED

#include <cpu.h>

#define MEM_SIZE (1 << 15)
#define LABELS_MAX (256)

#define GP0	RGS_GP0
#define GP1	RGS_GP1
#define GP2	RGS_GP2
#define GP3 RGS_GP3
#define GP4	RGS_GP4
#define GP5	RGS_GP5
#define GP6 RGS_GP6
#define LR	RGS_GP6
#define PC	RGS_PC

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

struct globals_struct
{
	int new_instruction;
	instruction* mem;
	int mem_size;
	label* labels;
	int labels_amount;
};

extern struct globals_struct globals;
	
void lbl(const char* const name);
void org(int loc);

//operations
void mov(const int dst, const int src);
void mov_on_zero(const int dst, const int src);
void mov_on_equal(const int dst, const int src);
void mov_on_not_zero(const int dst, const int src);
void mov_on_not_equal(const int dst, const int src);
void mov_on_not_carry(const int dst, const int src);
void mov_on_less_than(const int dst, const int src);
void mov_on_not_carry_or_zero(const int dst, const int src);
void mov_on_less_than_or_equal(const int dst, const int src);
void mov_literal(const int dst, const int literal);
void mov_literal_labeled(const int dst, const char* const name);
void add(const int dst, const int a, const int b);
void sub(const int dst, const int a, const int b);
void and(const int dst, const int a, const int b);
void or(const int dst, const int a, const int b);
void xor(const int dst, const int a, const int b);
void shiftl(const int dst, const int src);
void shiftr(const int dst, const int src);
void load(const int dst, const int src);
void store(const int dst, const int src);
void call(const int addr_reg);
void cmp(const int a, const int b);

//pseudo operations
void jump(const char* const name);
void halt();

#endif
