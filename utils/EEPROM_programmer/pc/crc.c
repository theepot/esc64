#include <stdint.h>
#include "crc.h"

static uint16_t crc16_update(uint16_t crc, uint8_t a)
{
	int i;

	crc ^= a;
	for (i = 0; i < 8; ++i)
	{
		if (crc & 1)
		crc = (crc >> 1) ^ CRC_POLYNOMIAL;
		else
		crc = (crc >> 1);
	}

	return crc;
}

uint16_t crc16(const uint8_t* data, int len)
{
	int n;
	uint16_t result = CRC_INITIAL_VALUE;
	for(n = 0; n < len; n++)
	{
		result = crc16_update(result, data[n]);
	}
	return result;
}
