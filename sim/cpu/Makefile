all: cpu.vcd

urom.lst: gen_urom_image
	./gen_urom_image > urom.lst
	
gen_urom_image: gen_urom_image.c ../../uassembler/uassembler.h cpu.h
	gcc -g -Wall -lm -o gen_urom_image -I../../uassembler gen_urom_image.c

cpu: cpu.v ../*/*.v ../globals.v
	iverilog -DALL_STRUCT -Wall cpu.v -o cpu
	
cpu.vcd: cpu urom.lst ram.lst
	-rm dump0.lst
	-rm dump1.lst
	./cpu
	
ram.lst: gen_program_ram
	./gen_program_ram > ram.lst

gen_program_ram: mini_assembler.c
	gcc -g -lm -Wall mini_assembler.c -o gen_program_ram
