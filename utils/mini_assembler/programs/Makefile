.PHONY: all
PROGRAMS = $(wildcard *.c) $(wildcard cpu-tests/*.c)
all: $(patsubst %.c,%.lst,${PROGRAMS})

%.lst: gen_%
	./$< > $@

gen_%: %.c ../mini_assembler.c ../mini_assembler.h
	gcc -I.. -I../../../lib/CInstrInfo/src -I../../../lib/cpu -g -lm -Wall $< ../mini_assembler.c -o $@

