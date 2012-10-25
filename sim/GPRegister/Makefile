.PHONY: all
all: GPRegister.test GPRegister_s.test

GPRegister.test: GPRegister.v GPRegister_test.v
	iverilog -Wall -o GPRegister.test GPRegister_test.v

GPRegister_s.test: GPRegister_s.v GPRegister_test.v
	iverilog -DGPREGISTER_STRUCT -Wall -o GPRegister_s.test GPRegister_test.v
