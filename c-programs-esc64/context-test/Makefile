.SUFFIXES:
.SECONDARY:

CO				:= ../context

PROGRAM			:= co-test
OBJECTS			:= main.o co-save.o
CRT_PATH		:= ../../crt
GPP_FLAGS		:= 
CFLAGS			:= -Isrc -I$(CO)/include
STDESC			:= ../stdesc
STDESC_OBJS		:= varargs.o putchar.o generic-printf.o printf.o fputs.o sprintf.o delay.o
ESCIO			:= ../escio
LINK			:= $(CO)/build/context.o

.PHONY: all
all: esc64_lst

.PHONY: clean
clean: esc64_clean

include ../esc64-build-v2.mk

