.PHONY: all
all: mseq.test
#mseq_s.test

mseq.test: mSeq.v mSeq_test.v ../globals.v
	iverilog -Wall -o mseq.test mSeq_test.v

#mseq_s.test: mSeq_s.v mSeq_test.v ../globals.v ../74xxx/octRegister_74273.v ../74xxx/Quad2To1Mux_74157.v
#	iverilog -Wall -o mseq_s.test -DMSEQ_STRUCT mSeq_test.v
