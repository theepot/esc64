.PHONY: all
all: bootstrapper

bootstrapper: bootstrapper.c bootstrapper.h crc.c crc.h ../common.h
	gcc -Wall -I.. -g -DDEBUG -lserial -lm -o bootstrapper bootstrapper.c crc.c
