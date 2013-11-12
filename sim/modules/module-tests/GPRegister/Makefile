.PHONY: all
all: GPRegister.test GPRegister_s.test

GPRegister.test: GPRegister.v GPRegister_test.v ../globals.v
	iverilog -Wall -o GPRegister.test GPRegister_test.v

GPRegister_s.test: GPRegister_s.v GPRegister_test.v ../74xxx/OctThreeState_74541.v ../74xxx/QuadCounter_74163.v ../globals.v
	iverilog -DGPREGISTER_STRUCT -Wall -o GPRegister_s.test GPRegister_test.v
