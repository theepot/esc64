#!/usr/bin/make -f

.PHONY: all
all:
	$(MAKE) -C conv-test clean > /dev/null; $(MAKE) -C conv-test test
	#$(MAKE) -C div-test  clean > /dev/null; $(MAKE) -C div-test test
	$(MAKE) -C mul-test  clean > /dev/null; $(MAKE) -C mul-test test
	$(MAKE) -C shift-test  clean > /dev/null; $(MAKE) -C shift-test test
	$(MAKE) -C switch-test  clean > /dev/null; $(MAKE) -C switch-test test

