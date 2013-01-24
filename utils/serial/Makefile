.PHONY: all
all: libserial.a

serial.o: serial.c
	gcc -g -Wall -c serial.c -o $@

libserial.a: serial.o
	ar rcs libserial.a serial.o

