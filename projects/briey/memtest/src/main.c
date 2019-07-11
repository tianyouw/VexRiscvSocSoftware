#include <stdint.h>
#include <briey.h>

#define PATTERN  0b10101010u

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
    register uint32_t currByte = 0;
    register uint8_t testByte;

    heapStart = (uint8_t*)&_heap_start;
    heapEnd = (uint8_t*)&_heap_end;

    // Write pattern
    while(&heapStart[currByte] < heapEnd) {
        // Write ~PATTERN on odd indices and PATTERN on even
        heapStart[currByte] = ((currByte & 0x01u) == 0x0) ? PATTERN : (uint8_t) ~PATTERN;
        currByte++;
    }

    if (state == 1) {
        heapStart[currByte - 1] = PATTERN + 1; // Cause a failure on even numbered runs
    }

    currByte = 0;

    // Read pattern
    while(&heapStart[currByte] < heapEnd) {
        testByte = heapStart[currByte];
        if (((currByte & 0x01u) == 0x0 && testByte != PATTERN) || (((currByte & 0x01u) == 0x1) && testByte != (uint8_t) ~PATTERN)) {
            fail();
        }
        currByte++;
    }

    pass();
}

void irqCallback(){
    state ^= 1; // Toggle state
    asm("jal x0,main"); // Jump directly back to main()
}
