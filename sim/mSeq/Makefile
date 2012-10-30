.PHONY: all
all: mseq.test mseq_s.test

mseq.test: mSeq.v mSeq_test.v ../globals.v
	iverilog -Wall -o mseq.test mSeq_test.v

mseq_s.test: mSeq_s.v mSeq_test.v ../globals.v
	iverilog -Wall -o mseq_s.test -DMSEQ_STRUCT mSeq_test.v
