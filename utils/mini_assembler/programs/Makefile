.PHONY: all
all: $(patsubst %.c,%.lst,$(wildcard *.c))

%.lst: gen_%
	./$< > $@

gen_%: %.c ../mini_assembler.c ../mini_assembler.h
	gcc -I.. -I/home/kasper/git/esc64/lib/CInstrInfo/src -I../../../lib/cpu -g -lm -Wall $< ../mini_assembler.c -o $@

