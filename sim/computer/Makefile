all: computer.vcd


computer: computer.v ../*/*.v ../globals.v
	iverilog -DALL_STRUCT -Wall computer.v -o computer
	
computer.vcd: computer urom.lst ram.lst
	-rm memBottomDump.lst
	-rm memTopDump.lst
	./computer
	
ram.lst: gen_program_ram
	./gen_program_ram > ram.lst

gen_program_ram: mini_assembler.c $(MINI_ASSEMBLER_TARGET) mini_assembler.h
	gcc -I. -I../../common -g -lm -Wall $(MINI_ASSEMBLER_TARGET) mini_assembler.c -o gen_program_ram
