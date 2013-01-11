.PHONY: all
all: urom_stable.lst

CFLAGS				= -Wall -g
LFLAGS				= -Wall -g

bin_table.o: bin_table.c bin_table.h
	gcc -o $@ $(CFLAGS) $<

uassembler.o: uassembler.c uassembler.h bin_table.h
	gcc -o $@ $(CFLAGS) $<

gen_%.o: gen_%.c bin_table.h uassembler.h
	gcc -o $@ $(CFLAGS) $<

%.lst: gen_%
	./$< > $@

gen_%: gen_%.o uassembler.o bin_table.o
	gcc -o $@ $(LFLAGS) $^

