.SUFFIXES:

.SECONDARY:


OBJECTS		:= esccrt.o
PP			:= $(foreach F,$(OBJECTS),$(subst .o,.pp.s,$(F)))

.PHONY: all
all: $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(PP)
	rm -f $(OBJECTS)

%.pp.s: %.s
	gpp -n +c ";" "\n" -o $@ $^

%.o: %.pp.s
	esc-as $@ $^

