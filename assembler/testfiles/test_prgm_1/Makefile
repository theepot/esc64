.PHONY: all
all: program.lst

.PHONY: clean
clean:
	rm -f program.lst
	rm -f program
	rm -f copy.o
	rm -f main.o
	rm -f mem.o

program.lst: program
	esc-exetolst program.lst program

program: copy.o main.o mem.o
	esc-ln program copy.o main.o mem.o

%.o: %.asm
	esc-as $@ $^

