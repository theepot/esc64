.SUFFIXES:
.SECONDARY:

BASE64			:= ../base64
GLORKZ			:= glorkz
CLEANFILES		:= setup src/gen_data.c

PROGRAM			:= advent
OBJECTS			:= gen_data.o subr.o wizard.o vocab.o save.o main.o done.o extern.h crc.o init.o io.o
CRT_PATH		:= ../../crt
GPP_FLAGS		:= 
CFLAGS			:= -I$(BASE64)/include
STDESC			:= ../stdesc
STDESC_OBJS		:= atoi.o rand.o fgets.o toupper.o puts.o itoa.o isdigit.o getchar.o fputs.o putchar.o generic-printf.o printf.o
ESCIO			:= ../escio
LINK			:= $(BASE64)/base64enc.o $(BASE64)/base64dec.o

src/gen_data.c: setup $(GLORKZ)
	./setup $(GLORKZ) > $@

setup: src/setup.c src/hdr.h
	gcc -Wall -Isrc -o $@ src/setup.c

include ../esc64-build.mk

