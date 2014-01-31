.SUFFIXES:

.SECONDARY:

SRC			:= src
OBJECTS		:= vio.o esctest.o
PP			:= $(foreach F,$(OBJECTS),$(subst .o,.pp.s,$(F)))
LCC_S		:= $(foreach F,$(OBJECTS),$(subst .o,.lcc.s,$(F)))

CRT_PATH	:= ../../crt
CRT_O		:= $(CRT_PATH)/esccrt.o

INCLUDES	:= $(CRT_PATH)
CFLAGS		:= -DTARGET_ESC64 -Iinclude

.PHONY: all
all: $(OBJECTS) linuxtest.o

linuxtest.o: $(SRC)/esctest.c
	gcc -c -Wall -Wextra -Iinclude -o $@ $<

.PHONY: clean
clean:
	rm -f $(PROGRAM)
	rm -f $(LST)
	rm -f $(PP)
	rm -f $(LCC_S)
	rm -f $(OBJECTS)
	rm -f linuxtest.o

$(LST): $(PROGRAM)
	esc-exetolst $@ $^

$(PROGRAM): $(OBJECTS)
	esc-ln $@ $^ $(CRT_O) > esc-ln-stdout.log

%.lcc.s: src/%.c
	lcc -S -c $(CFLAGS) -Wf-target=esc64 -o $@ $^

GPP_CMD=gpp -n +c ";" "\n" -o $@ $^ -I$(INCLUDES)

%.pp.s: %.lcc.s
	$(GPP_CMD)

%.pp.s: $(SRC)/%.s
	$(GPP_CMD)

%.o: %.pp.s
	esc-as $@ $^ > esc-as-stdout.log

