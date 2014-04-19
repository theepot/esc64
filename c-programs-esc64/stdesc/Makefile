.SUFFIXES:
.SECONDARY:

OBJECTS			:= strlen.o strncmp.o itoa.o atoi.o strchr.o isdigit.o rand.o fgets.o toupper.o puts.o fputs.o getchar.o varargs.o putchar.o memset.o generic-printf.o sprintf.o printf.o delay.o
CRT_PATH		:= ../../crt
GPP_FLAGS		:= 
CFLAGS			:= -Isrc
STDESC			:= ../stdesc
STDESC_OBJS		:= 
ESCIO			:= ../escio
LINK			:=

.PHONY: all
all: esc64_objs

.PHONY: clean
clean: esc64_clean

include ../esc64-build-v2.mk

