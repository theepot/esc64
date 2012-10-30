.PHONY: all
all: pc.test pc_s.test

pc.test: pc.v pc_test.v
	iverilog -Wall -o pc.test pc_test.v

pc_s.test: pc_s.v pc_test.v ../74xxx/QuadCounter_74163.v ../74xxx/OctThreeState_74541.v
	iverilog -Wall -o pc_s.test -DPC_STRUCT pc_test.v
