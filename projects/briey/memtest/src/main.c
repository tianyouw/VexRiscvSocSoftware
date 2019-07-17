#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <briey.h>

#define PATTERN       (0b10101010U)

#define UART_DATA_LEN (8U)
#define UART_BAUD     (115200U)

extern void flushDataCache(); // From /vga/src/crt.S
extern char _heap_start, _heap_end, _memTree_start, _memTree_end;
volatile uint8_t state = 0;

void fail(uint32_t errorAddr){
    GPIO_A_BASE->OUTPUT = ~0x000000FF ^ 0x04000000; // All red LEDs (not LEDG[7:0], and toggle LEDG[8])
    printf("Error detected at addr 0x%x\r\n", errorAddr);
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

    register uint32_t currByte = 0;
    register uint8_t testByte;

    uint8_t *heapStart = (uint8_t *)&_heap_start;
    uint8_t *heapEnd = (uint8_t *)&_heap_end;
    uint8_t *treeStart = (uint8_t *)&_memTree_start;
    uint8_t *treeEnd = (uint8_t *)&_memTree_end;

    printf("heapStart: 0x%x\r\n", heapStart);
    printf("heapEnd:   0x%x\r\n", heapEnd);
    printf("treeStart: 0x%x\r\n", treeStart);
    printf("treeEnd:   0x%x\r\n", treeEnd);

    printf("MemTest BEGIN\r\n");
    printf("Testing %u bytes of tree\r\n", treeEnd - treeStart);
    printf("Should all be zeroed before main() is called\r\n");
    // Flush D$ before reading back SDRAM
    flushDataCache();

    printf("Reading...\r\n");
    // Read pattern
    while(&treeStart[currByte] < treeEnd) {
        testByte = treeStart[currByte];
        if (testByte != 0U) {
            printf("Read back 0x%x, should be 0x%x\r\n", testByte, 0);
            fail(&treeStart[currByte]);
        }
        currByte++;
    }

    treePass();
    currByte = 0;

    printf("Testing %u bytes of heap\r\n", heapEnd - heapStart);
    printf("Writing...\r\n");

    // Write pattern
    while(&heapStart[currByte] < heapEnd) {
        // Write ~PATTERN on odd indices and PATTERN on even
        heapStart[currByte] = ((currByte & 1U) == 0) ? PATTERN : (uint8_t) ~PATTERN;
        currByte++;
    }

     // Cause a failure on second run
    if (state == 1U) {
        heapStart[currByte - 1] = PATTERN + 1; // Set the last byte in the heap to an unexpected value
        printf("Injecting error at addr 0x%x\r\n", &heapStart[currByte - 1]);
    }

    // Flush D$ before reading back SDRAM
    flushDataCache();
    currByte = 0;

    printf("Reading...\r\n");
    // Read pattern
    while(&heapStart[currByte] < heapEnd) {
        testByte = heapStart[currByte];
        if (((currByte & 1U) == 0) && (testByte != PATTERN)) {
            printf("Read back 0x%x, should be 0x%x\r\n", testByte, PATTERN);
            fail(&heapStart[currByte]);
        }
        if (((currByte & 1U) == 1) && (testByte != (uint8_t) ~PATTERN)) {
            printf("Read back 0x%x, should be 0x%x\r\n", testByte, (uint8_t) ~PATTERN);
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
