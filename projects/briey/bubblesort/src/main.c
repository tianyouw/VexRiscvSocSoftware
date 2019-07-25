#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <briey.h>

#define SORT_SIZE 16  // Size of the array to sort

#define UART_DATA_LEN (8U)
#define UART_BAUD     (115200U)

extern char _heap_start, _heap_end, _memTree_start, _memTree_end;
static const uint32_t inputArray[SORT_SIZE] = {80, 82, 9, 69, 43, 7, 59, 12, 78, 26, 35, 50, 62, 3, 88, 14};

void fail() {
    GPIO_A_BASE->OUTPUT = ~0x000000FF ^ 0x04000000; // All red LEDs (not LEDG[7:0], and toggle LEDG[8])
    printf("Bubblesort FAIL\r\n");
    while (1) {};
}

void pass() {
    GPIO_A_BASE->OUTPUT = 0x000000FF; // All green LEDs
    printf("Bubblesort PASS\r\n");
    while (1) {};
}

void test(uint32_t array[], uint32_t size) {
  for (uint32_t i = 0; i < size - 1; i++) {
    if (array[i] > array[i + 1]) {
        printf("Error: sortArray[%d] = %d > sortArray[%d] = %d\r\n", i, array[i], i + 1, array[i + 1]);
        return fail();
    }
  }
  return pass();
}

void bubbleSort(uint32_t array[], uint32_t size) {
    uint32_t temp;
    uint32_t iter = 0;

    for (uint32_t i = 0; i < size; i++) {
        for (uint32_t j = 1; j < (size - i); j++) {
           if (array[j - 1] > array[j]) {
                temp = array[j - 1];
                array[j - 1] = array[j];
                array[j] = temp;
           }
           iter++;
        }
    }

    printf("Bubblesort complete after %d iterations.\r\n", iter);
}

void printArray(uint32_t array[], uint32_t size) {
    printf("[%d", array[0]);
    for (uint32_t i = 1; i < size; i++) {
        printf(", %d", array[i]);
    }
    printf("]\r\n");
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

    uint32_t sortArray[SORT_SIZE];
    memcpy(sortArray, inputArray, sizeof(uint32_t) * SORT_SIZE);

    printf("Bubblesort BEGIN\r\n\r\n");

    printf("Input array:\r\n");
    printArray(sortArray, SORT_SIZE);

    printf("\r\n");
    bubbleSort(sortArray, SORT_SIZE);
    printf("\r\n");

    printf("Result array:\r\n");
    printArray(sortArray, SORT_SIZE);
    printf("\r\n");

    test(sortArray, SORT_SIZE);
}

void irqCallback() {
    // Required but unused
}
