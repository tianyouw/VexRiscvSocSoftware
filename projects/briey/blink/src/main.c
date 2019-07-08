#include <stdint.h>
#include <briey.h>

void main() {
    GPIO_A_BASE->OUTPUT_ENABLE = 0xFFFFFFFF;
    GPIO_A_BASE->OUTPUT = 0x00000000;

    while(1){
        for(uint32_t idx = 0;idx < 500000;idx++) asm volatile("nop");                              // Hardcoded 500K * 3 (~30ms) cycle delay
        GPIO_A_BASE->OUTPUT = (GPIO_A_BASE->OUTPUT & ~0xFF) | ((GPIO_A_BASE->OUTPUT + 1) & 0xFF);  // Counter on LED[7:0]
    }
}

void irqCallback(){
    // Required but unused function
}
