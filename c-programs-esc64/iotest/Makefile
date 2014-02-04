.SUFFIXES:

.SECONDARY:

SRC			:= src
PROGRAM		:= testprgm
LST			:= $(PROGRAM).lst
OBJECTS		:= test.o
PP			:= $(foreach F,$(OBJECTS),$(subst .o,.pp.s,$(F)))
LCC_S		:= $(foreach F,$(OBJECTS),$(subst .o,.lcc.s,$(F)))

CRT_PATH	:= ../../crt
CRT_O		:= $(CRT_PATH)/esccrt.o
CRT_INCLUDE	:= $(CRT_PATH)
ESC_TEST_O	:= ../common/esctest.o

GPP_FLAGS	:= -I$(CRT_PATH) -I.
CFLAGS		:= -I$(CRT_INCLUDE)

ESCIO		:= ../escio
ESCIO_OBJS	:= $(ESCIO)/escio.o
CFLAGS		+= -I$(ESCIO)/include

LINK		:= $(ESCIO_OBJS)

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
	esc-ln $@ $^ $(CRT_O) $(LINK) > esc-ln-stdout.log

%.lcc.s: src/%.c
	lcc -S -c -Wf-target=esc64 $(CFLAGS) -I$(SRC) -DTARGET_ESC64 -o $@ $^

GPP_CMD=gpp -n +c ";" "\n" -o $@ $^ $(GPP_FLAGS)

%.pp.s: %.lcc.s
	$(GPP_CMD)

%.pp.s: $(SRC)/%.s
	$(GPP_CMD)

%.o: %.pp.s
	esc-as $@ $^ > esc-as-stdout.log

