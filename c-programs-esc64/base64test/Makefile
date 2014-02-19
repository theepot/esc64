.SUFFIXES:
.SECONDARY:

BASE64			:= ../base64

PROGRAM			:= testprgm
OBJECTS			:= test.o
CRT_PATH		:= ../../crt
GPP_FLAGS		:= 
CFLAGS			:= -Iinclude -I$(BASE64)/include
STDESC			:= ../stdesc
STDESC_OBJS		:= puts.o
ESCIO			:= ../escio
LINK			:= $(BASE64)/build/base64_enc.o $(BASE64)/build/base64_dec.o

include ../esc64-build.mk

