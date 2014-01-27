.SUFFIXES:

.SECONDARY:


SRC			:= src
PROGRAM		:= conv_esc64
LST			:= $(PROGRAM).lst
OBJECTS		:= conv.o globals.o
PP			:= $(foreach F,$(OBJECTS),$(subst .o,.pp.s,$(F)))
LCC_S		:= $(foreach F,$(OBJECTS),$(subst .o,.lcc.s,$(F)))

CRT_PATH	:= ../../crt
CRT_O		:= $(CRT_PATH)/esccrt.o

INCLUDES	:= $(CRT_PATH)


.PHONY: all
all: $(LST) conv_x86linux

.PHONY: test
test: esc64r.log x86linuxr.log
	meld esc64r.log x86linuxr.log

esc64r.log: $(LST)
	echo "hello" > $@

x86linuxr.log: conv_x86linux
	./conv_x86linux > $@

.PHONY: clean
clean:
	rm -f $(PROGRAM)
	rm -f $(LST)
	rm -f $(PP)
	rm -f $(LCC_S)
	rm -f $(OBJECTS)

conv_x86linux: $(SRC)/conv.c
	gcc -g -Wall -I. -o $@ $^

$(LST): $(PROGRAM)
	esc-exetolst $@ $^

$(PROGRAM): $(OBJECTS)
	esc-ln $@ $^ $(CRT_O) > esc-ln-stdout.log

%.lcc.s: src/%.c
	lcc -S -c -Wf-target=esc64 -I. -DTARGET_ESC64 -o $@ $^
#	lcc -S -c '-Wf-g1,;' -Wf-target=esc64 -I. -DTARGET_ESC64 -o $@ $^

GPP_CMD=gpp -n +c ";" "\n" -o $@ $^ -I$(INCLUDES)

%.pp.s: %.lcc.s
	$(GPP_CMD)

%.pp.s: $(SRC)/%.s
	$(GPP_CMD)

%.o: %.pp.s
	esc-as $@ $^ > esc-as-stdout.log

