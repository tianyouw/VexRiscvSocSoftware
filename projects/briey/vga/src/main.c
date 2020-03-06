#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <briey.h>

// Needed so that simulation will start at a reasonable time; comment out for FPGA
// #ifndef NO_TREE_INIT
// #define NO_TREE_INIT
// #endif

#define RES_X 640
#define RES_Y 480

//#define RES_X 160
//#define RES_Y 120

//#define RES_X 48
//#define RES_Y 32

// __attribute__ ((section (".noinit"))) __attribute__ ((aligned (4*8))) uint16_t vgaFramebuffer[RES_Y][RES_X];
__attribute__ ((section (".noinit"))) __attribute__ ((aligned (4*8))) uint16_t dimmy[4][4];
__attribute__ ((section (".noinit"))) __attribute__ ((aligned (4*8))) uint16_t vgaFramebuffer[RES_Y][RES_X];


extern void flushDataCache(uint32_t dummy);

int main() {
	Uart_Config uartConfig;
	uartConfig.dataLength = 8;
	uartConfig.parity = ODD;
	uartConfig.stop = ONE;
	uartConfig.clockDivider = 50000000/8/115200-1;
	uart_applyConfig(UART,&uartConfig);

    vga_stop(VGA_BASE);
    VGA_BASE->TIMING = vga_h640_v480_r60_de2;  // vga_simRes   vga_h640_v480_r60 vga_simRes_h160_v120
    VGA_BASE->FRAME_SIZE = RES_X*RES_Y*2-1;
    VGA_BASE->FRAME_BASE = (uint32_t)vgaFramebuffer;
    vga_run(VGA_BASE);

    int quarter_offset = (RES_Y * RES_X) / 4;

    uint16_t read_data;
    while(1){
        uint16_t *ptr = &vgaFramebuffer[0][0];

        for(uint32_t y = 0;y < RES_Y/2;y++){
            for(uint32_t x = 0;x < RES_X/2;x++){
                read_data = uart_read(UART);
                read_data = (uart_read(UART) << 8) | read_data;\

                // uart_write(UART, read_data);

                vgaFramebuffer[y][x] = read_data;
                vgaFramebuffer[y + RES_Y/2][x] = read_data;
                vgaFramebuffer[y][x + RES_X/2] = read_data;
                vgaFramebuffer[y + RES_Y/2][x + RES_X/2] = read_data;

                // *ptr = read_data;              
                // ptr++;
            }
        }
        flushDataCache(0);
    }
}

// OLD STUFF
    // uint32_t offset = 0;
    // uint16_t rgb_count = 0;
    // // while(1){
    // 	uint16_t *ptr = &vgaFramebuffer[0][0];

    //     if (rgb_count == 3) rgb_count = 0;

    // 	for(uint32_t y = 0;y < RES_Y;y++){
    // 		uint32_t c = (((y + offset) & 0x1F) << 6);
    //     	for(uint32_t x = 0;x < RES_X;x++){
    //     		*ptr = ((uint16_t)(x & 0x1F)) + c;

    //             // switch (rgb_count) {
                    // case 0: // blue
                    //     *ptr = 0x0000F800;
                    //     break;
                    // case 1: // green
                    //     *ptr = 0x000007C0;
                    //     break;
                    // case 2: // red
                    //     *ptr = 0x0000001F;
                    //     break;
                    // default:
                    //     *ptr = 0xFFFFFFFF;
    //             //         break;
    //             // }
    //     		ptr++;
    //     	}
    // 	}
    // 	offset+=4;
    // 	flushDataCache(0);
    //     // switch (rgb_count) {
    //     //     case 0: // red
    //     //         uart_write(UART, '0');
    //     //         break;
    //     //     case 1: // green
    //     //         uart_write(UART, '1');
    //     //         break;
    //     //     case 2: // blue
    //     //         uart_write(UART, '2');
    //     //         break;
    //     //     default:
    //     //         uart_write(UART, '?');
    //     //         break;
    //     // }
    // 	uart_write(UART, '\n');
    //     rgb_count++;
    // // }
    // while(1) {}


void irqCallback(){

}