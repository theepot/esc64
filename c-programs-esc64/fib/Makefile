.SUFFIXES:

.SECONDARY:

SRC			:= src
PROGRAM		:= fib
LST			:= $(PROGRAM).lst
OBJECTS		:= fib.o globals.o
PP			:= $(foreach F,$(OBJECTS),$(subst .o,.pp.s,$(F)))
LCC_S		:= $(foreach F,$(OBJECTS),$(subst .o,.lcc.s,$(F)))

CRT_PATH	:= ../../crt
CRT_O		:= $(CRT_PATH)/esccrt.o

INCLUDES	:= $(CRT_PATH)


.PHONY: all
all: $(LST)

.PHONY: clean
clean:
	rm -f $(PROGRAM)
	rm -f $(LST)
	rm -f $(PP)
	rm -f $(LCC_S)
	rm -f $(OBJECTS)

$(LST): $(PROGRAM)
	esc-exetolst $@ $^

$(PROGRAM): $(OBJECTS)
	esc-ln $@ $^ $(CRT_O) > esc-ln-stdout.log

%.lcc.s: src/%.c
	lcc -S -c -Wf-target=esc64 -o $@ $^

GPP_CMD=gpp -n +c ";" "\n" -o $@ $^ -I$(INCLUDES)

%.pp.s: %.lcc.s
	$(GPP_CMD)

%.pp.s: $(SRC)/%.s
	$(GPP_CMD)

%.o: %.pp.s
	esc-as $@ $^ > esc-as-stdout.log

