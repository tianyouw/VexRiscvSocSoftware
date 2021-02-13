#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <briey.h>

#include <ascon.h>

#define UART_DATA_LEN (8U)
#define UART_BAUD     (115200U)

void main() {
    struct ascon_param param;
    ascon_data_block_t original;
    ascon_data_block_t encrypted;
    ascon_data_block_t decrypted;
    ascon_tag_t tag;
    Uart_Config uartConfig;
    uartConfig.dataLength = UART_DATA_LEN;
    uartConfig.parity = NONE;
    uartConfig.stop = ONE;
    uartConfig.clockDivider = (CORE_HZ / UART_DATA_LEN / UART_BAUD) - 1;
    uart_applyConfig(UART,&uartConfig);

    printf("Starting Ascon test\n");
    printf("Ascon connectivity check returned %d\n", ascon_is_sane());

    original[0] = 0x1234;
    original[1] = 0x5678;
    original[2] = 0x9ABC;
    original[3] = 0xDEF0;

    param.key_in[0] = 0x117;
    param.key_in[1] = 0x42;
    param.key_in[2] = 0x4529;
    param.key_in[3] = 0xCAFE;

    param.nonce_in[0] = 0x1;
    param.nonce_in[1] = 0x0;
    param.nonce_in[2] = 0x0;
    param.nonce_in[3] = 0x0;
    param.associated_size = 0;

    param.data_in = &original;
    param.data_out = &encrypted;
    param.data_size = 1;

    printf("Starting encryption\n");

    ascon_encrypt(&param);

    for (size_t i = 0; i < WIDE_LEN; i++) {
        tag[i] = param.tag_out[i];
    }
    param.data_in = &encrypted;
    param.data_out = &decrypted;

    printf("Starting decryption\n");
    ascon_decrypt(&param);

    int passed = 1;
    for (size_t i = 0; i < WIDE_LEN; i++) {
        printf("Expected: %lx; Actual: %lx\n", original[i], decrypted[i]);

        if (original[i] != decrypted[i]) {
            passed = 0;
        }
    }

    for (size_t i = 0; i < WIDE_LEN; i++) {
        printf("Expected tag: %lx; Actual tag: %lx\n", tag[i], param.tag_out[i]);

        if (tag[i] != param.tag_out[i]) {
            passed = 0;
        }
    }



    if (passed) {
        printf("Ascon test PASSED\n");
    } else {
        printf("Ascon test FAILED\n");
    }

    while(1);
}

void irqCallback(){
    // Do nothing
}
