#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <briey.h>

#ifdef USE_BYTES
    #define PATTERN   (0x55U)       // Repeating 0b0101...
    #define TEST_T    uint8_t
#else
    #define PATTERN   (0x55555555U)
    #define TEST_T    uint32_t
#endif

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

    register uint32_t currDataIdx = 0;
    register TEST_T   testData;
    register TEST_T   currPattern;

    volatile TEST_T *heapStart = (TEST_T *)&_heap_start;
    register TEST_T *heapEnd   = (TEST_T *)&_heap_end;
    volatile TEST_T *treeStart = (TEST_T *)&_memTree_start;
    register TEST_T *treeEnd   = (TEST_T *)&_memTree_end;

    printf("heapStart: 0x%x\r\n", heapStart);
    printf("heapEnd:   0x%x\r\n", heapEnd);
    printf("treeStart: 0x%x\r\n", treeStart);
    printf("treeEnd:   0x%x\r\n", treeEnd);

    printf("MemTest BEGIN\r\n");
    printf("%s mode\r\n", sizeof(testData) == 1 ? "byte" : "word");

    #ifndef NO_TREE_TEST // Set TREE=no in memtest/makefile to also disable tree region zeroing
        printf("Testing 0x%x bytes of tree\r\n", (uint8_t *)treeEnd - (uint8_t *)treeStart);
        printf("Should all be zeroed before main() is called\r\n");
        // Flush D$ before reading back SDRAM
        flushDataCache();

        printf("Reading...\r\n");
        // Read pattern
        while(&treeStart[currDataIdx] < treeEnd) {
            testData = treeStart[currDataIdx];
            if (testData != 0U) {
                printf("Read back 0x%x, should be 0x%x\r\n", testData, 0);
                fail(&treeStart[currDataIdx]);
            }
            currDataIdx++;
        }

        treePass();
        currDataIdx = 0;
    #endif // NO_TREE_INIT

    printf("Testing 0x%x bytes of heap\r\n", (uint8_t *)heapEnd - (uint8_t *)heapStart);

    // Write pattern, flush cache, read pattern, repeat until end of heap
    while(&heapStart[currDataIdx] < heapEnd) {
        // Write ~PATTERN on odd indices and PATTERN on even
        currPattern = ((currDataIdx & 1U) == 0) ? PATTERN : (TEST_T) ~PATTERN;
        heapStart[currDataIdx] = currPattern;
        flushDataCache();
        testData = heapStart[currDataIdx];
        if (testData != currPattern) {
            printf("Read back 0x%x, should be 0x%x\r\n", testData, currPattern);
            fail(&heapStart[currDataIdx]);
        }
        currDataIdx++;
    }

    pass();
}

void irqCallback(){
    // Required but not used
}
