.PHONY: all
all: EEPROM_programmer

COMMON_DIR=..
LIB_SERIAL_DIR=../../serial

EEPROM_programmer: EEPROM_programmer.c EEPROM_programmer.h crc.c crc.h ../common.h
	gcc -o EEPROM_programmer EEPROM_programmer.c crc.c -Wall -I$(COMMON_DIR) -I$(LIB_SERIAL_DIR) -L$(LIB_SERIAL_DIR) -lserial -g -DDEBUG -lm
