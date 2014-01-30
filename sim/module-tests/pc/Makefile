.PHONY: all
all: test.vvp

SRC = $(wildcard *_test.v)
DEPENDENCIES_DIRS = ../../modules ../../ICs
DEPENDENCIES = $(foreach i, $(DEPENDENCIES_DIRS), $(wildcard $(i)/*.v))
INCLUDE_FLAGS = $(foreach i, $(DEPENDENCIES_DIRS), -I$(i))

test.vvp: $(SRC) $(DEPENDENCIES)
	iverilog -Wall -DWAVEDUMPFILE=\"test.vcd\" $(INCLUDE_FLAGS) -o$@ $<
