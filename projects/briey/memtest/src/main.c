#include <stdint.h>
#include <briey.h>

#define PATTERN (0b10101010u)

extern void flushDataCache(); // From /vga/src/crt.S
extern int _heap_start, _heap_end;
volatile uint8_t state = 0;

void fail(){
    GPIO_A_BASE->OUTPUT = 0xFFFFFFFF ^ 0x04000000 ^ 0x000000FF; // All red LEDs
    while (1) {};
}

void pass(){
    GPIO_A_BASE->OUTPUT = 0x000000FF; // All green LEDs
    while (1) {};
}

void main() {
    GPIO_A_BASE->OUTPUT_ENABLE = 0xFFFFFFFF;
    GPIO_A_BASE->OUTPUT = 0x04000000; // LEDG8

    uint8_t *heapStart;
    uint8_t *heapEnd;
    register uint32_t currByte = 0;
    register uint8_t testByte;

    heapStart = (uint8_t*)&_heap_start;
    heapEnd = (uint8_t*)&_heap_end;

    // Write pattern
    while(&heapStart[currByte] < heapEnd) {
        // Write ~PATTERN on odd indices and PATTERN on even
        heapStart[currByte] = ((currByte & 1u) == 0) ? PATTERN : (uint8_t) ~PATTERN;
        currByte++;
    }

    if (state == 1u) {
        heapStart[currByte - 1] = PATTERN + 1; // Cause a failure on even numbered runs
    }

    // Flush D$ before reading back SDRAM
    flushDataCache();
    currByte = 0;

    // Read pattern
    while(&heapStart[currByte] < heapEnd) {
        testByte = heapStart[currByte];
        if (((currByte & 1u) == 0) && (testByte != PATTERN)) {
            fail();
        }
        if (((currByte & 1u) == 1) && (testByte != (uint8_t) ~PATTERN)) {
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
