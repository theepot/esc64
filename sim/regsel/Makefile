.PHONY: all
all: regsel.test regsel_s.test

regsel.test: regsel.v regsel_test.v ../globals.v
	iverilog -Wall -o regsel.test regsel_test.v

regsel_s.test: regsel_s.v regsel_test.v ../74xxx/ThreeToEightDecoder_74138.v ../74xxx/Dual4To1Mux_74153.v ../74xxx/Quad2To1Mux_74157.v ../globals.v
	iverilog -Wall -o regsel_s.test -DREGSEL_STRUCT regsel_test.v

