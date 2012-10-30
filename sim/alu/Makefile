.PHONY:all
all: alu.test alu_s.test

alu.test: alu_test.v alu.v alu_common.v ../globals.v
	iverilog -o alu.test -Wall alu_test.v

alu_s.test: alu_test.v alu_s.v ../74xxx/ALU_74181.v ../74xxx/OctThreeState_74541.v ../74xxx/OctInputOrNor_744078.v ../74xxx/Quad2InputAnd_7408.v ../74xxx/Quad2To1Mux_74157.v ../74xxx/Dual4To1Mux_74153.v ../74xxx/HexInvertor_7404.v alu_common.v ../globals.v
	iverilog -o alu_s.test -DALU_STRUCT -Wall alu_test.v
