TOP := $(dir $(lastword $(MAKEFILE_LIST)))

#targets
LST				:= $(PROGRAM).lst
OBJ_PATHS		:= $(foreach F,$(OBJECTS),build/$(F))

#for cleaning
PP				:= $(foreach F,$(OBJECTS),build/$(subst .o,.pp.s,$(F)))
LCC_S			:= $(foreach F,$(OBJECTS),build/$(subst .o,.lcc.s,$(F)))

#crt
CFLAGS			+= -I$(CRT_PATH)
LINK			+= $(CRT_PATH)/esccrt.o
M4_FLAGS		+= -I$(CRT_PATH)

#stdesc
LINK			+= $(foreach F,$(STDESC_OBJS),$(STDESC)/build/$(F))
CFLAGS			+= -I$(STDESC)/include

#escio
CFLAGS			+= -I$(ESCIO)/include
LINK			+= $(ESCIO)/build/escio.o

#misc
M4_FLAGS		+= -Isrc
CFLAGS			+= -DTARGET_ESC64
CLEANFILES		+= 	$(PROGRAM) $(LST) $(PP) $(LCC_S) $(OBJ_PATHS)

#handy macro's
RM_FAIL=(rm $@; false)
M4_CMD=m4 $^ $(M4_FLAGS) > $@ || $(RM_FAIL)

##### phony rules for users to depends on #####
.PHONY: esc64_clean
esc64_clean:
	rm -f $(CLEANFILES)
	rmdir build 2> /dev/null; true

.PHONY: esc64_exe
esc64_exe: $(PROGRAM)

.PHONY: esc64_lst
esc64_lst: $(LST)

.PHONY: esc64_objs
esc64_objs: $(OBJ_PATHS)

##### internal rules #####
$(LST): $(PROGRAM)
	esc-exetolst $@ $^

#link
$(PROGRAM): $(OBJ_PATHS) | build
	esc-ln $@ `$(TOP)/genobjpaths.rb $(CRT_PATH) $^ $(LINK)` > esc-ln-stdout.log || $(RM_FAIL)

#compile
build/%.lcc.s: src/%.c | build
	lcc -S -c -Wf-target=esc64 $(CFLAGS) -o $@ $^ || $(RM_FAIL)

#preprocess assembly
build/%.pp.s: build/%.lcc.s | build
	$(M4_CMD)

build/%.pp.s: src/%.s | build
	$(M4_CMD)

#assemble
build/%.o: build/%.pp.s | build
	esc-as $@ $^ > esc-as-stdout.log || $(RM_FAIL)

build:
	mkdir -p $@
