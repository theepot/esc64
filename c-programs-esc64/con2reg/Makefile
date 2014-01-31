.SUFFIXES:

.SECONDARY:

SRC			:= src
PROGRAM		:= test
LST			:= $(PROGRAM).lst
OBJECTS		:= test.o
PP			:= $(foreach F,$(OBJECTS),$(subst .o,.pp.s,$(F)))
LCC_S		:= $(foreach F,$(OBJECTS),$(subst .o,.lcc.s,$(F)))

CRT_PATH	:= ../../crt
CRT_O		:= $(CRT_PATH)/esccrt.o

INCLUDES	:= $(CRT_PATH)
CFLAGS		:= -DTARGET_ESC64

COMMON		:= ../common
COMMON_INC	:= $(COMMON)/include
COMMON_LINK	:= $(COMMON)/vio.o $(COMMON)/esctest.o
OBJECTS		+= $(COMMON_LINK)

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
	lcc -S -c $(CFLAGS) -Wf-target=esc64 -o $@ $^ -I$(COMMON_INC)

GPP_CMD=gpp -n +c ";" "\n" -o $@ $^ -I$(INCLUDES)

%.pp.s: %.lcc.s
	$(GPP_CMD)

%.pp.s: $(SRC)/%.s
	$(GPP_CMD)

%.o: %.pp.s
	esc-as $@ $^ > esc-as-stdout.log

