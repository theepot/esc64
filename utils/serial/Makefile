.PHONY: all
all: serial.a

serial.o: serial.c
	gcc -g -Wall -c serial.c -o $@

serial.a: serial.o
	ar rcs serial.a serial.o

