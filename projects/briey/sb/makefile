PROJ_NAME=riscv_tests_sb
DEBUG=no
NO_OPT=yes
BENCH=no
MULDIV=no
# "yes" enables tree mem test and tree zeroing
# run "make tree=no" to disable this
TREE=no

SRCS = $(wildcard ./*.S)

LDSCRIPT = ../libs/linker.ld
INC += -I../libs/riscv_tests/

include ../../../resources/gcc.mk
include ../../../resources/subproject.mk
