#include <stdint.h>
#include <briey.h>

#define PATTERN  ((uint8_t) 0b10101010)

extern int _heap_start, _heap_end;
volatile int state = 0;

void fail(){
    GPIO_A_BASE->OUTPUT = 0xFFFFFF00; // All red LEDs
    while (1) {};
}

void pass(){
    GPIO_A_BASE->OUTPUT = 0x000000FF; // All green LEDs
    while (1) {};
}

void main() {
    GPIO_A_BASE->OUTPUT_ENABLE = 0xFFFFFFFF;
    GPIO_A_BASE->OUTPUT = 0x00000000;

    uint8_t *heapStart;
    uint8_t *heapEnd;
    uint32_t currByte = 0;

    heapStart = (uint8_t*)&_heap_start;
    heapEnd = (uint8_t*)&_heap_end;

    // Write pattern
    while(&heapStart[currByte] < heapEnd) {
        heapStart[currByte] = PATTERN;
        heapStart[currByte + 1] = ~PATTERN;
        currByte = currByte + 2;
    }

    if (state == 1) {
        heapStart[currByte] = ~PATTERN; // Cause a failure on even numbered runs
    }

    currByte = 0;

    // Read pattern
    while(&heapStart[currByte] < heapEnd) {
        if (heapStart[currByte] != PATTERN) {
            fail();
        }
        if (heapStart[currByte + 1] != ~PATTERN) {
            fail();
        }
        currByte = currByte + 2;
    }

    pass();
}

void irqCallback(){
    state ^= 1; // Toggle state
    asm("jal x0,main"); // Jump directly back to main()
}
