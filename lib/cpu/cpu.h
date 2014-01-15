#ifndef _CPU_H
#define _CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#define UROM_ADDR_WIDTH			13
#define OPCODE_WIDTH 			7

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

#define DEV_READ_LENGTH			50
#define DEV_WRITE_LENGTH		50
#define MEM_READ_LENGTH			1
#define MEM_WRITE_LENGTH		1

typedef enum register_id_t {
	RGS_GP0 = 0,
	RGS_GP1 = 1,
	RGS_GP2 = 2,
	RGS_GP3 = 3,
	RGS_GP4 = 4,
	RGS_GP5 = 5,
	RGS_SP	= 5,
	RGS_GP6 = 6,
	RGS_LR	= 6,
	RGS_PC  = 7,
} register_id_t;


#ifdef __cplusplus
}
#endif


#endif
