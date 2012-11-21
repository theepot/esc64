SHELL = /bin/bash

.PHONY: all

all:
	@mkdir -p build
	pushd build; gcc -Wall -g -I../include  -c ../src/*.c ../unit_tests/*.c; popd
	@mkdir -p bin
	pushd bin; gcc -Wall -g -I../include ../build/*.o; popd;

