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
ESC_TEST_O	:= ../common/esctest.o

GPP_FLAGS	:= -I$(CRT_PATH) -I.
CFLAGS		:=

COMMON		:= ../common
COMMON_OBJS	:= $(COMMON)/esctest.o $(COMMON)/vio.o
COMMON_INC	:= $(COMMON)/include
COMMON_SRC	:= $(COMMON)/src/esctest.c

LINUX_SRC	:= $(COMMON_SRC) $(addprefix $(SRC)/,$(foreach F,$(OBJECTS),$(subst .o,.c,$(F))))
LINUX_PRGM	:= linux_$(PROGRAM)

.PHONY: all
all: linux esc

.PHONY: linux
linux: $(LINUX_PRGM)

$(LINUX_PRGM): $(LINUX_SRC)
	gcc -Wall -o linux_$(PROGRAM) $(LINUX_SRC) -I$(COMMON_INC) -I$(SRC)

.PHONY: esc
esc: $(LST)

.PHONY: clean
clean:
	rm -f $(PROGRAM)
	rm -f $(LST)
	rm -f $(PP)
	rm -f $(LCC_S)
	rm -f $(OBJECTS)
	rm -f $(LINUX_PRGM)

$(LST): $(PROGRAM)
	esc-exetolst $@ $^

$(PROGRAM): $(OBJECTS)
	esc-ln $@ $^ $(CRT_O) $(COMMON_OBJS) > esc-ln-stdout.log

%.lcc.s: src/%.c
	lcc -S -c -Wf-target=esc64 -I$(COMMON_INC) -I$(SRC) -DTARGET_ESC64 -o $@ $^

GPP_CMD=gpp -n +c ";" "\n" -o $@ $^ $(GPP_FLAGS)

%.pp.s: %.lcc.s
	$(GPP_CMD)

%.pp.s: $(SRC)/%.s
	$(GPP_CMD)

%.o: %.pp.s
	esc-as $@ $^ > esc-as-stdout.log

