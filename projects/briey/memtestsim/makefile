PROJ_NAME=memtestsim
DEBUG=no
NO_OPT=yes
BENCH=no
MULDIV=no

# "yes" enables tree mem test and tree zeroing
# run "make tree=no" to disable this
TREE=no

# "byte"/"word" sets memtest to use byte/word sized transfers
VAR_SIZE=byte

SRCS = 	$(wildcard src/*.c)   \
        $(wildcard src/*.cpp) \
        $(wildcard src/*.S)   \
        ../libs/stdlib.c

LDSCRIPT = ../libs/linker.ld
INC += -I../../../libs/
INC += -I../libs/

include ../../../resources/gcc.mk
include ../../../resources/subproject.mk
