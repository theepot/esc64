.SUFFIXES:

PROGRAM		:= program
LST			:= $(PROGRAM).lst
OBJECTS		:= copy.o main.o mem.o
PP			:= $(foreach F,$(OBJECTS),$(subst .o,.pp.s,$(F)))

.PHONY: all
all: program.lst

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
	gpp -n -o $@ $^

%.o: %.pp.s
	esc-as $@ $^

