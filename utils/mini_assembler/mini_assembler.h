#ifndef MINI_ASSEMBLER_INCLUDED
#define MINI_ASSEMBLER_INCLUDED

#include <cpu.h>
#include <instr_info.h>

#define MEM_SIZE (1 << 15)
#define LABELS_MAX (256)

#define GP0	(RGS_GP0)
#define GP1	RGS_GP1
#define GP2	RGS_GP2
#define GP3 RGS_GP3
#define GP4	RGS_GP4
#define GP5	RGS_GP5
#define GP6 RGS_GP6
#define LR	RGS_LR
#define PC	RGS_PC
#define SP	RGS_SP

typedef struct {
	opcode_t opcode;
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
void mov_on_notcarry_and_notzero(const int dst, const int src);
void mov_on_notcarry_and_zero(const int dst, const int src);
void mov_on_notcarry(const int dst, const int src);
void mov_on_carry_and_notzero(const int dst, const int src);
void mov_on_notzero(const int dst, const int src);
void mov_on_notcarry_or_notzero(const int dst, const int src);
void mov_on_carry_and_zero(const int dst, const int src);
void mov_on_zero(const int dst, const int src);
void mov_on_notcarry_or_zero(const int dst, const int src);
void mov_on_carry(const int dst, const int src);
void mov_on_carry_or_notzero(const int dst, const int src);
void mov_on_carry_or_zero(const int dst, const int src);
void mov_literal(const int dst, const int literal);
void mov_literal_on_notcarry_and_notzero(const int dst, const int literal);
void mov_literal_on_notcarry_and_zero(const int dst, const int literal);
void mov_literal_on_notcarry(const int dst, const int literal);
void mov_literal_on_carry_and_notzero(const int dst, const int literal);
void mov_literal_on_notzero(const int dst, const int literal);
void mov_literal_on_notcarry_or_notzero(const int dst, const int literal);
void mov_literal_on_carry_and_zero(const int dst, const int literal);
void mov_literal_on_zero(const int dst, const int literal);
void mov_literal_on_notcarry_or_zero(const int dst, const int literal);
void mov_literal_on_carry(const int dst, const int literal);
void mov_literal_on_carry_or_notzero(const int dst, const int literal);
void mov_literal_on_carry_or_zero(const int dst, const int literal);
void mov_literal_labeled(const int dst, const char* const name);
void mov_literal_on_notcarry_and_notzero_labeled(const int dst, const char* const name);
void mov_literal_on_notcarry_and_zero_labeled(const int dst, const char* const name);
void mov_literal_on_notcarry_labeled(const int dst, const char* const name);
void mov_literal_on_carry_and_notzero_labeled(const int dst, const char* const name);
void mov_literal_on_notzero_labeled(const int dst, const char* const name);
void mov_literal_on_notcarry_or_notzero_labeled(const int dst, const char* const name);
void mov_literal_on_carry_and_zero_labeled(const int dst, const char* const name);
void mov_literal_on_zero_labeled(const int dst, const char* const name);
void mov_literal_on_notcarry_or_zero_labeled(const int dst, const char* const name);
void mov_literal_on_carry_labeled(const int dst, const char* const name);
void mov_literal_on_carry_or_notzero_labeled(const int dst, const char* const name);
void mov_literal_on_carry_or_zero_labeled(const int dst, const char* const name);
void add(const int dst, const int a, const int b);
void add_with_carry(const int dst, const int a, const int b);
void sub(const int dst, const int a, const int b);
void sub_with_carry(const int dst, const int a, const int b);
void and(const int dst, const int a, const int b);
void or(const int dst, const int a, const int b);
void xor(const int dst, const int a, const int b);
void not(const int dst, const int a);
void inc(const int dst, const int a);
void dec(const int dst, const int a);
void shiftl(const int dst, const int src);
void shiftr(const int dst, const int src);
void load(const int dst, const int src);
void store(const int dst, const int src);
void load_byte(const int dst, const int src);
void store_byte(const int dst, const int src);
void call(const int addr_reg);
void call_literal(const int addr);
void call_literal_labeled(const char* const name);
void in(const int dst, const int src);
void out(const int dst, const int src);
void cmp(const int a, const int b);
void push(int r);
void pop(int r);
void halt();

//pseudo operations
void jump(const char* const name);

void save_all(void);
void restore_all(void);



#endif
