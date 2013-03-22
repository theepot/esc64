.PHONY: all
all: computer.vcd

.PHONY: clean
clean:
	rm -f computer
	rm -f computer.vcd
	rm -f computer.vvp
	rm -f interactive.o
	rm -f interactive.vpi
	rm -f ram_out.lst

computer.vvp: computer.v ../*/*.v ../globals.v
	iverilog -DALL_STRUCT -o$@ $^

#computer: computer.v ../*/*.v ../globals.v
#	iverilog -DALL_STRUCT -Wall computer.v -o computer

computer.vcd: computer.vvp urom.lst ram.lst interactive.vpi
	vvp -M. -minteractive computer.vvp
	
#computer.vcd: computer urom.lst ram.lst
#	./computer
	
interactive.vpi: interactive.c
	iverilog-vpi $^

