.SUFFIXES:

.SECONDARY:

PROGRAM		:= rpnread
LST			:= $(PROGRAM).lst
OBJECTS		:= read.o
PP			:= $(foreach F,$(OBJECTS),$(subst .o,.pp.s,$(F)))

.PHONY: all
all: $(PROGRAM).lst

.PHONY: clean
clean:
	rm -f $(PROGRAM)
	rm -f $(LST)
	rm -f $(PP)
	rm -f $(OBJECTS)

$(LST): $(PROGRAM)
	esc-exetolst $@ $^

$(PROGRAM): $(OBJECTS)
	esc-ln $@ $^

%.pp.s: %.s
	gpp -n +c ";" "\n" -o $@ $^

%.o: %.pp.s
	esc-as $@ $^

