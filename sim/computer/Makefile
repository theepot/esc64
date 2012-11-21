all: computer.vcd

urom.lst: gen_urom_image
	./gen_urom_image > urom.lst
	
gen_urom_image: gen_urom_image.c ../../uassembler/uassembler.h ../cpu/cpu.h
	gcc -g -Wall -lm -o gen_urom_image -I../../uassembler gen_urom_image.c

computer: computer.v ../*/*.v ../globals.v
	iverilog -DALL_STRUCT -Wall computer.v -o computer
	
computer.vcd: computer urom.lst ram.lst
	-rm memBottomDump.lst
	-rm memTopDump.lst
	./computer
	
ram.lst: gen_program_ram
	./gen_program_ram > ram.lst

gen_program_ram: mini_assembler.c
	gcc -g -lm -Wall mini_assembler.c -o gen_program_ram
