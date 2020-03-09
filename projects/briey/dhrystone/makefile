PROJ_NAME=dhrystone
DEBUG=no
BENCH=no
MULDIV=yes
TREE=no


CFLAGS += -fno-inline -fno-common -g
LDFLAGS += -lc

CFLAGS +=  -DPREALLOCATE=1 -DHOST_DEBUG=0 -DMSC_CLOCK 


SRCS = 	src/main.c src/dhry_1.c src/dhry_2.c src/crt.S  ../libs/stdlib.c 
LDFLAGS += -lc #Used to get memcpy/strcmp stuff only

INC += -I../../../libs/
INC += -I../libs/
LDSCRIPT = ../libs/linker.ld


include ../../../resources/gcc.mk
include ../../../resources/subproject.mk


			
