.PHONY: all
all: bootstrapper

COMMON_DIR=..
LIB_SERIAL_DIR=../../serial

bootstrapper: bootstrapper.c bootstrapper.h crc.c crc.h ../common.h
	gcc -o bootstrapper bootstrapper.c crc.c -Wall -I$(COMMON_DIR) -I$(LIB_SERIAL_DIR) -L$(LIB_SERIAL_DIR) -lserial -g -DDEBUG -lm
