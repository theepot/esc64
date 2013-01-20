#ifndef __CRC_H
#define __CRC_H
#include <stdint.h>

#define CRC_POLYNOMIAL		0xA001
#define CRC_INITIAL_VALUE	0xFFFF

uint16_t crc16(const uint8_t* data, int len);

#endif
