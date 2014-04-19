.SUFFIXES:
.SECONDARY:

PROGRAM			:= delaytest
OBJECTS			:= main.o delay.o
CRT_PATH		:= ../../crt
GPP_FLAGS		:= 
CFLAGS			:= 
STDESC			:= ../stdesc
STDESC_OBJS		:= printf.o putchar.o generic-printf.o varargs.o puts.o delay.o
ESCIO			:= ../escio
LINK			:=

.PHONY: all
all: esc64_lst

.PHONY: clean
clean: esc64_clean

include ../esc64-build-v2.mk

