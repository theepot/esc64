#targets
LST				:= $(PROGRAM).lst
OBJ_PATHS		:= $(foreach F,$(OBJECTS),build/$(F))

#for cleaning
PP				:= $(foreach F,$(OBJECTS),build/$(subst .o,.pp.s,$(F)))
LCC_S			:= $(foreach F,$(OBJECTS),build/$(subst .o,.lcc.s,$(F)))

#crt
CFLAGS			+= -I$(CRT_PATH)
LINK			+= $(CRT_PATH)/esccrt.o
GPP_FLAGS		+= -I$(CRT_PATH)

#stdesc
LINK			+= $(foreach F,$(STDESC_OBJS),$(STDESC)/$(F))
CFLAGS			+= -I$(STDESC)/include

#escio
CFLAGS			+= -I$(ESCIO)/include
LINK			+= $(ESCIO)/build/escio.o

#misc
GPP_FLAGS		+= -Isrc
CFLAGS			+= -DTARGET_ESC64
CLEANFILES		+= 	$(PROGRAM) $(LST) $(PP) $(LCC_S) $(OBJ_PATHS)
ifeq ($(strip $(PROGRAM)),)
ALL_DEP			:= $(OBJ_PATHS)
else
ALL_DEP			:= $(LST)
endif

#handy macro's
RM_FAIL=(rm $@; false)
GPP_CMD=gpp -n +c ";" "\n" -o $@ $^ $(GPP_FLAGS) || $(RM_FAIL)

##### rules #####
.PHONY: all
all: $(ALL_DEP)

.PHONY: clean
clean:
	rm -f $(CLEANFILES)
	rmdir build 2> /dev/null; true

$(LST): $(PROGRAM)
	esc-exetolst $@ $^

#link
$(PROGRAM): $(OBJ_PATHS) | build
	esc-ln $@ $^ $(LINK) > esc-ln-stdout.log || $(RM_FAIL)

#compile
build/%.lcc.s: src/%.c | build
	lcc -S -c -Wf-target=esc64 $(CFLAGS) -o $@ $^ || $(RM_FAIL)

#preprocess assembly
build/%.pp.s: build/%.lcc.s | build
	$(GPP_CMD)

build/%.pp.s: src/%.s | build
	$(GPP_CMD)

#assemble
build/%.o: build/%.pp.s | build
	esc-as $@ $^ > esc-as-stdout.log || $(RM_FAIL)

build:
	mkdir -p $@
