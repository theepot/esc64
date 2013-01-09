#ifndef __EEPROM_FEEDER_H
#define __EEPROM_FEEDER_H

#include <stdint.h>

//---------------eeprom settings
#define EEPROM_BLOCK_SIZE		64
#define EEPROM_SIZE				8192
#define EEPROM_WIDTH			8 //command_write function does not support more than 8!!
#define EEPROM_BLOCKS			(EEPROM_SIZE/EEPROM_BLOCK_SIZE)

//---------------EEPROM mem definitons
#define EEPROM_ZERO				0
#define EEPROM_ONE				1
#define EEPROM_DONTCARE			2
#define EEPROM_UNKOWN			3

#define WAITING_NOTIFY_DIVIDER	1000 //in milliseconds


typedef uint8_t				EEPROM_t;
#define EEPROM_MEM_SIZE		sizeof(EEPROM_t) * EEPROM_SIZE * EEPROM_WIDTH



#endif
