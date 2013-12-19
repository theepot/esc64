.PHONY: all
all: bootstrapper

COMMON_DIR=..
LIB_SERIAL_DIR=../../serial
ESC64ASM_INCLUDE_DIR=../../../assembler/include
ESC64ASM_LIB_DIR=../../../assembler/lib

.PHONY: clean
clean:
	$(RM) bootstrapper

bootstrapper: bootstrapper.c bootstrapper.h crc.c crc.h ../common.h
	gcc -O0 -g3 -o bootstrapper bootstrapper.c crc.c -Wall -I. -I$(ESC64ASM_INCLUDE_DIR) -L$(ESC64ASM_LIB_DIR) -I$(COMMON_DIR) -I$(LIB_SERIAL_DIR) -L$(LIB_SERIAL_DIR) -lserial -lesc-asm-common -DDEBUG -lm
