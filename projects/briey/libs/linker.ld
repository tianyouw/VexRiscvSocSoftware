/*======================================================================*/
/* Proxy kernel linker script                                           */
/*======================================================================*/
/* This is the linker script used when building the proxy kernel. */

/*----------------------------------------------------------------------*/
/* Setup                                                                */
/*----------------------------------------------------------------------*/

/* The OUTPUT_ARCH command specifies the machine architecture where the
   argument is one of the names used in the BFD library. More
   specifically one of the entires in bfd/cpu-mips.c */

/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.
*/
OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv", "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(crtStart)

MEMORY {
  onChipRam : ORIGIN = 0x80000000, LENGTH = 32K
  sdram : ORIGIN = 0x40000000, LENGTH = 128M
}
_stack_size = DEFINED(_stack_size) ? _stack_size : 2K;
/*
_memTree_size = 128;
_heap_size = 32M - _stack_size - SIZEOF(.noinit);
*/

_memTree_size = 3 * LENGTH(sdram) / 4 ;  /* Size of mem. auth. tree */

SECTIONS /*TODO don't initialize useless things,  restore literal loading that use 2 instruction in place of onChipRam word */
{
  .vector : {
    *crt.o(.text);
  } > onChipRam

  .memory : {
    *(.text);
    end = .;
  } > onChipRam

  .rodata         :
  {
    *(.rdata)
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
  } > onChipRam

  .ctors          :
  {
    . = ALIGN(4);
    _ctors_start = .;
    KEEP(*(.init_array*))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    . = ALIGN(4);
    _ctors_end = .;
  } > onChipRam

  .data          :
  {
    *(.rdata)
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
    *(.data .data.*)
    *(.gnu.linkonce.d.*)
    . = ALIGN(8);
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
  } > onChipRam

  .bss (NOLOAD) : {
    . = ALIGN(4);
    /* This is used by startup routine (crt.S) to initialize the .bss section */
    _bss_start = .;
    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(4);
    _bss_end = .;
  } > onChipRam

  .noinit (NOLOAD) : {
      . = ALIGN(4);
      *(.noinit .noinit.*)
      . = ALIGN(4);    
  } > sdram

  _heap_size = LENGTH(sdram) - _stack_size - _memTree_size - SIZEOF(.noinit);  /* Fill rest of SDRAM with heap */

  ._user_heap (NOLOAD):
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    PROVIDE ( _heap_start = .);
    . = . + _heap_size;
    . = ALIGN(8);
    PROVIDE ( _heap_end = .);
  } > sdram

  ._stack (NOLOAD):
  {
    . = ALIGN(16);
    PROVIDE (_stack_end = .);
    . = . + _stack_size;
    . = ALIGN(16);
    PROVIDE (_stack_start = .);
  } > sdram

  .memTree (NOLOAD) : {
    . = ALIGN(4);
    /* This is used by the startup routine (crt.S) to initialize the .memTree section (for mem. auth.) */
    PROVIDE ( _memTree_start = . );
    . = . + _memTree_size;
    . = ALIGN(4);
    PROVIDE ( _memTree_end = . );
  } > sdram
}