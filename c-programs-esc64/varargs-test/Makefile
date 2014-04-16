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

COMMON		:= ../common
COMMON_OBJS	:= $(COMMON)/esctest.o $(COMMON)/vio.o
COMMON_INC	:= $(COMMON)/include
COMMON_SRC	:= $(COMMON)/src/esctest.c

LINUX_SRC	:= $(COMMON_SRC) $(addprefix $(SRC)/,$(foreach F,$(OBJECTS),$(subst .o,.c,$(F))))
LINUX_PRGM	:= linux_$(PROGRAM)

STDESC		:= ../stdesc
STDESC_OBJS	:= $(STDESC)/build/varargs.o
CFLAGS		+= -I$(STDESC)/include

ESCIO		:= ../escio
CFLAGS		+= -I$(ESCIO)/include
ESCIO_OBJS	:= $(ESCIO)/build/escio.o

LINUX_OUT	:= linux.log
ESC_OUT		:= esc.log

.PHONY: all
all: linux esc

.PHONY: test
test: $(ESC_OUT) $(LINUX_OUT)
	diff $(ESC_OUT) $(LINUX_OUT)

.PHONY: linux
linux: $(LINUX_PRGM)

$(LINUX_OUT): $(LINUX_PRGM)
	./$(LINUX_PRGM) > $@

$(ESC_OUT): $(LST)
	esc64vm --quit-after-halt --vos $@ -r $<

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
	rm -f $(LINUX_OUT) $(ESC_OUT)

$(LST): $(PROGRAM)
	esc-exetolst $@ $^

$(PROGRAM): $(OBJECTS)
	esc-ln $@ $^ $(CRT_O) $(STDESC_OBJS) $(ESCIO_OBJS) $(COMMON_OBJS) > esc-ln-stdout.log

%.lcc.s: src/%.c
	lcc -S -c -Wf-target=esc64 $(CFLAGS) -I$(COMMON_INC) -I$(SRC) -DTARGET_ESC64 -o $@ $^

#GPP_CMD=gpp -n +c ";" "\n" -o $@ $^ $(GPP_FLAGS)
GPP_CMD=m4 $^ $(GPP_FLAGS) > $@

%.pp.s: %.lcc.s
	$(GPP_CMD)

%.pp.s: $(SRC)/%.s
	$(GPP_CMD)

%.o: %.pp.s
	esc-as $@ $^ > esc-as-stdout.log

