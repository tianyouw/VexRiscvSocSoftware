PROJ_NAME=vga
DEBUG=no
NO_OPT=yes
BENCH=no
MULDIV=no
TREE=no


SRCS = 	$(wildcard src/*.c) \
	    $(wildcard src/*.cpp)  \
	    $(wildcard src/*.S)   \
        ../libs/stdlib.c    


LDSCRIPT = ../libs/linker.ld
INC += -I../../../libs/
INC += -I../libs/

include ../../../resources/gcc.mk
include ../../../resources/subproject.mk
