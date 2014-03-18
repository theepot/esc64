#!/usr/bin/make -f

#rebuilds the crt, c-programs/[common,escio,stdesc]

.PHONY: all
all:
	$(MAKE) -C crt clean > /dev/null; $(MAKE) -C crt all
	$(MAKE) -C c-programs-esc64/common clean > /dev/null; $(MAKE) -C c-programs-esc64/common
	$(MAKE) -C c-programs-esc64/escio clean > /dev/null; $(MAKE) -C c-programs-esc64/escio
	$(MAKE) -C c-programs-esc64/stdesc clean > /dev/null; $(MAKE) -C c-programs-esc64/stdesc
