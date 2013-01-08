#ifndef __COMMON_H
#define __COMMON_H

//--------------commands & replies
#define COMMAND_EREASE			0x01
#define COMMAND_WRITE			0x02
#define COMMAND_READ			0x03

#define COMMAND_XOR				0xAB //pattern the command should be XOR'ed with
#define REPLY_OK				0x01
#define REPLY_BAD_CRC			0x02
#define REPLY_ACTION_FAILED		0x03

#define HANDSHAKE_OUTGOING		0xB4 //handshake byte to microcontroller
#define HANDSHAKE_INCOMMING		0x3A //handshake byte to pc

#endif
