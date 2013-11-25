#ifndef _CPU_H
#define _CPU_H

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
#define RGS_LR				RGS_GP6
#define RGS_SP				RGS_GP5

#define RGS_LOADSRC_USEQ		0
#define RGS_LOADSRC_OP0 		1
#define RGS_OESRC_USEQ 			0
#define RGS_OESRC_OP0 			1
#define RGS_OESRC_OP1 			2
#define RGS_OESRC_OP2 			3

//these are the S3-S0 and M signals fed to the 74181 ALU in the form of: {S3, S2, S1, S0, M}
#define ALU_F_A				0x00//carry in: 0
#define ALU_F_B				0x15//carry in: 0
#define ALU_F_SUB			0x0C//carry in: 1
#define ALU_F_ADD			0x12//carry in: 0
#define ALU_F_NOT_A			0x01//carry in: 0
#define ALU_F_XOR			0x0D//carry in: 0
#define ALU_F_AND			0x17//carry in: 0
#define ALU_F_OR			0x1D//carry in: 0
#define ALU_F_A_PLUS_ONE	0x00//carry in: 1
#define ALU_F_A_MINUS_ONE	0x1E//carry in: 0
#define ALU_F_ZERO			0x07//carry in: x
#define ALU_F_SHIFT_LEFT	0x05//carry in: x
#define ALU_F_SHIFT_RIGHT	0x02//carry in: x

#define ALU_CSEL_UCIN			0
#define ALU_CSEL_SRCIN			1

#define	ERROR_WIRE_ILLEGAL_OPCODE		1
#define ERROR_WIRE_ILLEGAL_STATE		2

#define IO_READ_LENGTH			50
#define IO_WRITE_LENGTH			50
#define RAM_READ_LENGTH			1
#define RAM_WRITE_LENGTH		1


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
	op_inc = 9,
	op_dec = 10,
	op_or = 11,
	op_or_literal = 12,
	op_xor = 13,
	op_xor_literal = 14,
	op_and = 15,
	op_and_literal = 16,
	op_not = 17,
	op_shift_left_1 = 18,
	op_shift_left_2 = 19,
	op_shift_left_3 = 20,
	op_shift_left_4 = 21,
	op_shift_left_5 = 22,
	op_shift_left_6 = 23,
	op_shift_left_7 = 24,
	op_shift_left_8 = 25,
	op_shift_left_9 = 26,
	op_shift_left_10 = 27,
	op_shift_left_11 = 28,
	op_shift_left_12 = 29,
	op_shift_left_13 = 30,
	op_shift_left_14 = 31,
	op_shift_left_15 = 32,
	op_shift_right_1 = 33,
	op_shift_right_2 = 34,
	op_shift_right_3 = 35,
	op_shift_right_4 = 36,
	op_shift_right_5 = 37,
	op_shift_right_6 = 38,
	op_shift_right_7 = 39,
	op_shift_right_8 = 40,
	op_shift_right_9 = 41,
	op_shift_right_10 = 42,
	op_shift_right_11 = 43,
	op_shift_right_12 = 44,
	op_shift_right_13 = 45,
	op_shift_right_14 = 46,
	op_shift_right_15 = 47,
	op_mov = 48,
	op_mov_on_notcarry_and_notzero = 49,
	op_mov_on_notcarry_and_zero = 50,
	op_mov_on_notcarry = 51,
	op_mov_on_carry_and_notzero = 52,
	op_mov_on_notzero = 53,
	op_mov_on_notcarry_or_notzero = 55,
	op_mov_on_carry_and_zero = 56,
	op_mov_on_zero = 58,
	op_mov_on_notcarry_or_zero = 59,
	op_mov_on_carry = 60,
	op_mov_on_carry_or_notzero = 61,
	op_mov_on_carry_or_zero = 62,
	op_mov_literal = 63,
	op_mov_literal_on_notcarry_and_notzero = 64,
	op_mov_literal_on_notcarry_and_zero = 65,
	op_mov_literal_on_notcarry = 66,
	op_mov_literal_on_carry_and_notzero = 67,
	op_mov_literal_on_notzero = 68,
	op_mov_literal_on_notcarry_or_notzero = 70,
	op_mov_literal_on_carry_and_zero = 71,
	op_mov_literal_on_zero = 73,
	op_mov_literal_on_notcarry_or_zero = 74,
	op_mov_literal_on_carry = 75,
	op_mov_literal_on_carry_or_notzero = 76,
	op_mov_literal_on_carry_or_zero = 77,
	op_cmp = 78,
	op_cmp_literal = 79,
	op_cmp_literal_reverse = 80,
	op_load = 81,
	op_load_literal = 82,
	op_load_with_offset = 83,
	op_store = 84,
	op_store_literal = 85,
	op_store_with_offset = 86,
	op_call = 87,
	op_call_literal = 88,
	op_in = 89,
	op_out = 90,
	op_push = 91,
	op_pop = 92,
	op_halt = 127
} opcode;


#endif
