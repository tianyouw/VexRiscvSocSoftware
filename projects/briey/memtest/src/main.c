#include <stdint.h>
#include <stdlib.h>
#include <briey.h>

#define PATTERN       (0b10101010U)

#define UART_DATA_LEN (8U)
#define UART_BAUD     (115200U)

extern void flushDataCache(); // From /vga/src/crt.S
extern int _heap_start, _heap_end, _memTree_start, _memTree_end;
volatile uint8_t state = 0;

void fail(uint32_t errorAddr){
    GPIO_A_BASE->OUTPUT = ~0x000000FF ^ 0x04000000; // All red LEDs (not LEDG[7:0], and toggle LEDG[8])
    printf("Error detected at addr %d\r\n", errorAddr);
    while (1) {};
}

void pass(){
    GPIO_A_BASE->OUTPUT = 0x000000FF; // All green LEDs
    printf("MemTest PASS\r\n");
    while (1) {};
}

void treePass(){
    GPIO_A_BASE->OUTPUT = 0x0000000F; // Half of the green LEDs
    printf("MemTest Tree PASS\r\n");
}

void main() {
    GPIO_A_BASE->OUTPUT_ENABLE = 0xFFFFFFFF;
    GPIO_A_BASE->OUTPUT = 0x04000000; // LEDG8

    Uart_Config uartConfig;
    uartConfig.dataLength = UART_DATA_LEN;
    uartConfig.parity = NONE;
    uartConfig.stop = ONE;
    uartConfig.clockDivider = (CORE_HZ / UART_DATA_LEN / UART_BAUD) - 1;
    uart_applyConfig(UART,&uartConfig);

    uint8_t *heapStart;
    uint8_t *heapEnd;
    uint8_t *treeStart;
    uint8_t *treeEnd;
    register uint32_t currByte = 0;
    register uint8_t testByte;

    heapStart = (uint8_t*)&_heap_start;
    heapEnd = (uint8_t*)&_heap_end;
    treeStart = (uint8_t*)&_memTree_start;
    treeEnd = (uint8_t*)&_memTree_end;

    printf("MemTest BEGIN\r\n");
    printf("Testing %d bytes of tree\r\n", treeEnd - treeStart);
    printf("Should all be zeroed before main() is called\r\n");
    // Flush D$ before reading back SDRAM
    flushDataCache();

    printf("Reading...\r\n");
    // Read pattern
    while(&treeStart[currByte] < treeEnd) {
        testByte = treeStart[currByte];
        if (testByte != 0U) {
            fail(&treeStart[currByte]);
        }
        currByte++;
    }

    treePass();

    printf("Testing %d bytes of heap\r\n", heapEnd - heapStart);
    printf("Writing...\r\n");

    // Write pattern
    while(&heapStart[currByte] < heapEnd) {
        // Write ~PATTERN on odd indices and PATTERN on even
        heapStart[currByte] = ((currByte & 1u) == 0) ? PATTERN : (uint8_t) ~PATTERN;
        currByte++;
    }

    if (state == 1u) {
        heapStart[currByte - 1] = PATTERN + 1; // Cause a failure on even numbered runs
        printf("Injecting error at addr %d\r\n", &heapStart[currByte - 1]);
    }

    // Flush D$ before reading back SDRAM
    flushDataCache();
    currByte = 0;

    printf("Reading...\r\n");
    // Read pattern
    while(&heapStart[currByte] < heapEnd) {
        testByte = heapStart[currByte];
        if (((currByte & 1u) == 0) && (testByte != PATTERN)) {
            fail(&heapStart[currByte]);
        }
        if (((currByte & 1u) == 1) && (testByte != (uint8_t) ~PATTERN)) {
            fail(&heapStart[currByte]);
        }
        currByte++;
    }

    pass();
}

void irqCallback(){
    state ^= 1; // Toggle state
    asm("jal x0,main"); // Jump directly back to main()
}
