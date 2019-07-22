#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <briey.h>

// TODO: add flag for byte vs word tests
#define PATTERN       (0x55555555U) // Repeating 0b0101...

#define UART_DATA_LEN (8U)
#define UART_BAUD     (115200U)

extern void flushDataCache(); // From /vga/src/crt.S
extern char _heap_start, _heap_end, _memTree_start, _memTree_end;
volatile uint32_t state = 0;

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

    register uint32_t currWord = 0;
    register uint32_t testWord;

    register uint32_t *heapStart = (uint32_t *)&_heap_start;
    register uint32_t *heapEnd = (uint32_t *)&_heap_end;
    register uint32_t *treeStart = (uint32_t *)&_memTree_start;
    register uint32_t *treeEnd = (uint32_t *)&_memTree_end;

    printf("heapStart: 0x%x\r\n", heapStart);
    printf("heapEnd:   0x%x\r\n", heapEnd);
    printf("treeStart: 0x%x\r\n", treeStart);
    printf("treeEnd:   0x%x\r\n", treeEnd);

    printf("MemTest BEGIN\r\n");

    #ifndef NO_TREE_TEST // Set TREE=no in memtest/makefile to also disable tree region zeroing
        printf("Testing 0x%x bytes of tree\r\n", (uint8_t *)treeEnd - (uint8_t *)treeStart);
        printf("Should all be zeroed before main() is called\r\n");
        // Flush D$ before reading back SDRAM
        flushDataCache();

        printf("Reading...\r\n");
        // Read pattern
        while(&treeStart[currWord] < treeEnd) {
            testWord = treeStart[currWord];
            if (testWord != 0U) {
                printf("Read back 0x%x, should be 0x%x\r\n", testWord, 0);
                fail(&treeStart[currWord]);
            }
            currWord++;
        }

        treePass();
        currWord = 0;
    #endif // NO_TREE_INIT

    printf("Testing 0x%x bytes of heap\r\n", (uint8_t *)heapEnd - (uint8_t *)heapStart);
    printf("Writing...\r\n");

    // Write pattern
    while(&heapStart[currWord] < heapEnd) {
        // Write ~PATTERN on odd indices and PATTERN on even
        heapStart[currWord] = ((currWord & 1U) == 0) ? PATTERN : (uint32_t) ~PATTERN;
        currWord++;
    }

     // Cause a failure on second run
    if (state == 1U) {
        heapStart[currWord - 1] = PATTERN + 1; // Set the last byte in the heap to an unexpected value
        printf("Injecting error at addr 0x%x\r\n", &heapStart[currWord - 1]);
    }

    // Flush D$ before reading back SDRAM
    flushDataCache();
    currWord = 0;

    printf("Reading...\r\n");
    // Read pattern
    while(&heapStart[currWord] < heapEnd) {
        testWord = heapStart[currWord];
        if (((currWord & 1U) == 0) && (testWord != PATTERN)) {
            printf("Read back 0x%x, should be 0x%x\r\n", testWord, PATTERN);
            fail(&heapStart[currWord]);
        }
        if (((currWord & 1U) == 1) && (testWord != (uint32_t) ~PATTERN)) {
            printf("Read back 0x%x, should be 0x%x\r\n", testWord, (uint32_t) ~PATTERN);
            fail(&heapStart[currWord]);
        }
        currWord++;
    }

    pass();
}

void irqCallback(){
    state ^= 1; // Toggle state
    asm("jal x0,main"); // Jump directly back to main()
}
