.SUFFIXES:

.SECONDARY:

SRC			:= src
PROGRAM		:= testprgm
LST			:= $(PROGRAM).lst
OBJECTS		:= escio.o
PP			:= $(foreach F,$(OBJECTS),$(subst .o,.pp.s,$(F)))
LCC_S		:= $(foreach F,$(OBJECTS),$(subst .o,.lcc.s,$(F)))

CRT_PATH	:= ../../crt
CRT_O		:= $(CRT_PATH)/esccrt.o
ESC_TEST_O	:= ../common/esctest.o

GPP_FLAGS	:= -I$(CRT_PATH) -I.
CFLAGS		:=

COMMON		:= ../common
COMMON_OBJS	:= $(COMMON)/esctest.o $(COMMON)/vio.o
COMMON_INC	:= $(COMMON)/include
COMMON_SRC	:= $(COMMON)/src/esctest.c

.PHONY: all
all: $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(PROGRAM)
	rm -f $(LST)
	rm -f $(PP)
	rm -f $(LCC_S)
	rm -f $(OBJECTS)

%.lcc.s: src/%.c
	lcc -S -c -Wf-target=esc64 -I$(COMMON_INC) -I$(SRC) -DTARGET_ESC64 -o $@ $^

GPP_CMD=gpp -n +c ";" "\n" -o $@ $^ $(GPP_FLAGS)

%.pp.s: %.lcc.s
	$(GPP_CMD)

%.pp.s: $(SRC)/%.s
	$(GPP_CMD)

%.o: %.pp.s
	esc-as $@ $^ > esc-as-stdout.log

