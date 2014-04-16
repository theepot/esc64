.SUFFIXES:
.SECONDARY:

OBJECTS			:= base64_dec.o base64_enc.o
CRT_PATH		:= ../../crt
GPP_FLAGS		:= 
CFLAGS			:= -Iinclude
STDESC			:= ../stdesc
STDESC_OBJS		:= 
ESCIO			:= ../escio
LINK			:= 

.PHONY: all
all: esc64_objs

.PHONY: clean
clean: esc64_clean

include ../esc64-build-v2.mk

