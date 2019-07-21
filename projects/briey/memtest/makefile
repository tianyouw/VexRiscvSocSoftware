PROJ_NAME=memtest
DEBUG=no
NO_OPT=yes
BENCH=no
MULDIV=no
# "yes" enables tree mem test and tree zeroing
# run "make tree=no" to disable this
TREE=no

ifeq ($(TREE),no)
	CFLAGS += -DNO_TREE_TEST -DNO_TREE_INIT
endif

SRCS = 	$(wildcard src/*.c)   \
        $(wildcard src/*.cpp) \
        $(wildcard src/*.S)   \
        ../libs/stdlib.c

LDSCRIPT = ../libs/linker.ld
INC += -I../../../libs/
INC += -I../libs/

include ../../../resources/gcc.mk
include ../../../resources/subproject.mk
