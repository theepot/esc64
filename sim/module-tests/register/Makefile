.PHONY: all
all: test.vvp

TEST_SRC = $(wildcard *_test.v)
DEPENDENCIES_ABSTRACT = $(wildcard ../../abstract-modules/*.v) $(wildcard ../../common-modules/*.v)

test.vvp: $(TEST_SRC) $(DEPENDENCIES_ABSTRACT)
	iverilog -Wall -DWAVEDUMPFILE=\"test.vcd\" -I../../abstract-modules -I../../common-modules -o$@ $<
