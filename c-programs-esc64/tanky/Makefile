.SUFFIXES:
.SECONDARY:

PROGRAM			:= tanky
OBJECTS			:= angle2vec.o tanky.o
CRT_PATH		:= ../../crt
GPP_FLAGS		:= 
CFLAGS			:= -Isrc
STDESC			:= ../stdesc
STDESC_OBJS		:= varargs.o putchar.o generic-printf.o getchar.o printf.o delay.o fputs.o sprintf.o
ESCIO			:= ../escio
LINK			:=

PROGRAM_LINUX	:= tanky-linux

.PHONY: all
all: esc64_lst $(PROGRAM_LINUX)

.PHONY: clean
clean: esc64_clean linux_clean

include ../esc64-build-v2.mk

$(PROGRAM_LINUX):
	gcc -Wall -Isrc -o $@ src/*.c
	
.PHONY: linux_clean
linux_clean:
	rm -f $(PROGRAM_LINUX)
