.PHONY: all
all: sram.test sram_s.test


sram.test: sram.v sram_test.v mem.lst ../globals.v
	iverilog -Wall -o sram.test sram_test.v

sram_s.test: sram_s.v sram_test.v mem.lst ../globals.v
	iverilog -Wall -o sram_s.test -DSRAM_STRUCT sram_test.v
