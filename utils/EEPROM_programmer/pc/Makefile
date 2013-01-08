.PHONY: all
all: EEPROM_programmer

EEPROM_programmer: EEPROM_programmer.c EEPROM_programmer.h crc.c crc.h ../common.h
	gcc -Wall -g -DDEBUG -lserial -o EEPROM_programmer EEPROM_programmer.c crc.c
