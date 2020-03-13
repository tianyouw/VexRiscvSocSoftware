PROJ_NAME=vga
DEBUG=no
NO_OPT=yes
BENCH=no
MULDIV=no
TREE=yes

SRCS = 	$(wildcard src/*.c)   \
	    $(wildcard src/*.cpp) \
	    $(wildcard src/*.S)

LDSCRIPT = ../libs/linker.ld
INC += -I../../../libs/
INC += -I../libs/

include ../../../resources/gcc.mk
include ../../../resources/subproject.mk
