// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

#include <stdarg.h>
#include <stdint.h>
#include "briey.h"

extern char *malloc();
extern int printf(const char *format, ...);
extern void putchar(char c);

void putchar(char c)
{
    uart_write(UART, c);
}

extern char _heap_start;
extern int *_heap_size;
int heap_memory_used = 0;

char *malloc(int size)
{
    char *p = &_heap_start + heap_memory_used;
    // printf("[malloc(%d) -> %d (%d..%d)]", size, (int)p, heap_memory_used, heap_memory_used + size);
    heap_memory_used += size;
    if (heap_memory_used > &_heap_size)
        asm volatile ("ebreak");
    return p;
}

static void printf_c(int c)
{
    putchar(c);
}

static void printf_s(char *p)
{
    while (*p)
        putchar(*(p++));
}

static void printf_d(int val, int isSigned)
{
    char buffer[32];
    char *p = buffer;
    if ((val < 0) && isSigned) {
        printf_c('-');
        val = -val;
    }
    uint32_t myVal = (uint32_t) val;
    while (myVal || p == buffer) {
        *(p++) = '0' + myVal % 10;
        myVal = myVal / 10;
    }
    while (p != buffer)
        printf_c(*(--p));
}

static void printf_x(int val)
{
    char buffer[32];
    char hexChar;
    char *p = buffer;
    uint32_t myVal = (uint32_t) val;
    uint32_t len = 8;

    while (len--) {
        hexChar = myVal & 0x0F; // Mask least sig. hex digit
        *(p++) = hexChar > 9 ? (hexChar + 'A' - 10) : (hexChar + '0');
        myVal = myVal / 16;
    }
    while (p != buffer)
        printf_c(*(--p));
}

int printf(const char *format, ...)
{
    int i;
    va_list ap;

    va_start(ap, format);

    for (i = 0; format[i]; i++)
        if (format[i] == '%') {
            while (format[++i]) {
                if (format[i] == 'c') {
                    printf_c(va_arg(ap,int));
                    break;
                }
                if (format[i] == 's') {
                    printf_s(va_arg(ap,char*));
                    break;
                }
                if (format[i] == 'd') {
                    printf_d(va_arg(ap,int), 1);
                    break;
                }
                if (format[i] == 'u') {
                    printf_d(va_arg(ap,int), 0);
                    break;
                }
                if (format[i] == 'x') {
                    printf_x(va_arg(ap,int));
                    break;
                }
            }
        } else
            printf_c(format[i]);

    va_end(ap);
}


int puts(char *s)
{
  while (*s) {
    putchar(*s);
    s++;
  }
  putchar('\n');
  return 0;
}
