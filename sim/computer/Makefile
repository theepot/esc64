@PHONY: all
all: computer.vcd

computer: computer.v ../*/*.v ../globals.v
	iverilog -DALL_STRUCT -Wall computer.v -o computer
	
computer.vcd: computer urom.lst ram.lst
	-rm memBottomDump.lst
	-rm memTopDump.lst
	./computer
