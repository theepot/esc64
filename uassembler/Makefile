.PHONY: all
all: urom.lst

CFLAGS				= -Wall -g -I../common
LFLAGS				= -Wall -g

bin_table.o: bin_table.c bin_table.h
	gcc -c -o $@ $(CFLAGS) $<

uassembler.o: uassembler.c uassembler.h bin_table.h
	gcc -c -o $@ $(CFLAGS) $<

gen_%.o: gen_%.c bin_table.h uassembler.h
	gcc -c -o $@ $(CFLAGS) $<

%.lst: gen_%
	./$< > $@

gen_%: gen_%.o uassembler.o bin_table.o
	gcc -o $@ $(LFLAGS) $^
