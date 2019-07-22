#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <briey.h>

#define UART_DATA_LEN (8U)
#define UART_BAUD     (115200U)

extern char _heap_start, _heap_end, _memTree_start, _memTree_end;

void fail(){
    GPIO_A_BASE->OUTPUT = ~0x000000FF ^ 0x04000000; // All red LEDs (not LEDG[7:0], and toggle LEDG[8])
    printf("Bubblesort FAIL\r\n");
    while (1) {};
}

void pass(){
    GPIO_A_BASE->OUTPUT = 0x000000FF; // All green LEDs
    printf("Bubblesort PASS\r\n");
    while (1) {};
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

    pass();
}

void irqCallback(){

}
