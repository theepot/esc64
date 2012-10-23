all: cpu.vcd

urom.lst: gen_urom_image
	./gen_urom_image > urom.lst
	
gen_urom_image: gen_urom_image.c ../../uassembler/uassembler.h
	gcc -g -Wall -lm -o gen_urom_image -I../../uassembler gen_urom_image.c

cpu: cpu.v ../*/*.v
	iverilog -Wall cpu.v -o cpu

cpu.vcd: cpu urom.lst ram.lst
	./cpu
	
ram.lst: gen_program_ram
	./gen_program_ram > ram.lst

gen_program_ram: mini_assembler.c
	gcc -lm -Wall mini_assembler.c -o gen_program_ram
