#ifndef __CPU_H
#define __CPU_H

#define UROM_ADDR_WIDTH			13
#define OPCODE_WIDTH 			7

#define RGS_GP0				0
#define RGS_GP1				1
#define RGS_GP2				2
#define RGS_GP3				3
#define RGS_GP4				4
#define RGS_GP5				5
#define RGS_GP6				6
#define RGS_PC				7

#define RGS_LOADSRC_USEQ		0
#define RGS_LOADSRC_OP0 		1
#define RGS_OESRC_USEQ 			0
#define RGS_OESRC_OP0 			1
#define RGS_OESRC_OP1 			2
#define RGS_OESRC_OP2 			3

#define ALU_F_A				0
#define ALU_F_B				21
#define ALU_F_SUB			12
#define ALU_F_ADD			18
#define ALU_F_NOT			1
#define ALU_F_XOR			13
#define ALU_F_AND			23
#define ALU_F_OR			29
#define ALU_F_SHIFT_LEFT		5
#define ALU_F_SHIFT_RIGHT		2

#define ALU_CSEL_UCIN			0
#define ALU_CSEL_FCIN			1

#define	ERROR_WIRE_ILLEGAL_OPCODE		1
#define ERROR_WIRE_ILLEGAL_STATE		2

typedef enum opcode {
	op_reset = 0,
	op_add = 1,
	op_add_with_carry = 2,
	op_add_literal = 3,
	op_add_literal_with_carry = 4,
	op_sub = 5,
	op_sub_with_carry = 6,
	op_sub_literal = 7,
	op_sub_literal_with_carry = 8,
	op_sub_literal_reverse = 9,
	op_sub_literal_with_carry_reverse = 10,
	op_or = 11,
	op_or_literal = 12,
	op_xor = 13,
	op_xor_literal = 14,
	op_and = 15,
	op_and_literal = 16,
	op_shift_left = 17,
	op_shift_right = 18,
	op_mov = 19,
	op_mov_on_zero = 20,
	op_mov_on_not_zero = 21,
	op_mov_on_not_carry = 22,
	op_mov_on_not_carry_or_zero = 23,
	op_mov_literal = 24,
	op_mov_literal_on_zero = 25,
	op_mov_literal_on_not_zero = 26,
	op_mov_literal_on_not_carry = 27,
	op_mov_literal_on_not_carry_or_zero = 28,
	op_cmp = 29,
	op_cmp_literal = 30,
	op_cmp_literal_reverse = 31,
	op_load = 32,
	op_load_literal = 33,
	op_load_with_offset = 34,
	op_store = 36,
	op_store_literal = 37,
	op_store_with_offset = 38,
	op_call = 40,
	op_call_literal = 41
} opcode;



#endif
