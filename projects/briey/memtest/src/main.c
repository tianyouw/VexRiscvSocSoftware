#include <stdint.h>
#include <briey.h>

#define PATTERN  ((uint8_t) 0xFE)
#define TESTSIZE (0x1000)

extern int _heap_start;

void fail(){
    GPIO_A_BASE->OUTPUT = 0xFFFFFF00;
    while (1) {};
}

void pass(){
    GPIO_A_BASE->OUTPUT = 0x000000FF;
    while (1) {};
}

void main() {
    GPIO_A_BASE->OUTPUT_ENABLE = 0xFFFFFFFF;
    GPIO_A_BASE->OUTPUT = 0x00000000;

    uint8_t *heap;
    uint32_t currByte = 0;

    heap = (uint8_t*)&_heap_start;

    // Write pattern
    while(currByte < TESTSIZE) {
        heap[currByte] = PATTERN;
        currByte++;
    }

    currByte = 0;

    // Read pattern
    while(currByte < TESTSIZE) {
        if (heap[currByte] != PATTERN) {
            fail();
        }
        currByte++;
    }

    pass();
}

void irqCallback(){
    // Required but unused function
}
