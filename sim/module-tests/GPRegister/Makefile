.PHONY: all
all: test_abstract.vvp test_discrete.vvp

TEST_SRC = $(wildcard *_test.v)
DEPENDENCIES_ABSTRACT = $(wildcard ../../abstract-modules/*.v) $(wildcard ../../common-modules/*.v)
DEPENDENCIES_DISCRETE = $(wildcard ../../discrete-modules/*.v) $(wildcard ../../common-modules/*.v) $(wildcard ../../ICs/*.v)

test_abstract.vvp: $(TEST_SRC) $(DEPENDENCIES_ABSTRACT)
	iverilog -Wall -DWAVEDUMPFILE=\"test_abstract.vcd\" -I../../abstract-modules -I../../common-modules -o$@ $<

test_discrete.vvp: $(TEST_SRC) $(DEPENDENCIES_DISCRETE)
	iverilog -Wall -DWAVEDUMPFILE=\"test_discrete.vcd\" -I../../ICs -I../../discrete-modules -I../../common-modules -o$@ $<
