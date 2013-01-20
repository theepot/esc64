#ifndef __COMMON_H
#define __COMMON_H

//--------------commands & replies


#define COMMAND_XOR				0xAB //pattern the command should be XOR'ed with
#define COMMAND_UPLOAD			0x01
#define COMMAND_DOWNLOAD		0x02
#define COMMAND_START			0x03
#define COMMAND_STOP			0x04
#define COMMAND_SET_CLOCK		0x05
#define COMMAND_RESET			0x06
#define COMMAND_STEP			0x07
#define COMMAND_STEP_INSTR		0x08

#define REPLY_OK				0x01
#define REPLY_BAD_CRC			0x02
#define REPLY_ACTION_FAILED		0x03

#define HANDSHAKE_OUTGOING		0xB4 //handshake byte to microcontroller
#define HANDSHAKE_INCOMMING		0x3A //handshake byte to pc

#define SRAM_WIDTH				16
#define SRAM_DEPTH				(1 << 15)
#define SRAM_BLOCK_SIZE			128
#define SRAM_BLOCKS				(SRAM_DEPTH / SRAM_BLOCK_SIZE)
#define SRAM_WORD_SIZE			(SRAM_WIDTH / 8)

#define MAX_MCU_BUF_SIZE		1024

#endif
