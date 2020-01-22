#include "ascon.h"

#include <stdint.h>
#include <string.h>

/* Taken from Table 2 in https://github.com/IAIK/ascon_hardware/blob/master/doc/ascon_hw_doc.pdf */
static volatile struct {
	uint32_t unique_value;
	uint32_t status;
	uint32_t control;
	uint32_t scheduled_status;
	uint32_t key[4];
	uint32_t nonce[4];
	uint32_t data[2];
	uint32_t filler2[2];
	uint32_t tag[4];
} *const ASCON128_INTERFACE = (volatile void *) 0x40000000;

#define ASCON_CTRL_INIT (1U << 0)
#define ASCON_CTRL_ASSOCIATE (1U << 1)
#define ASCON_CTRL_ENCRYPT (1U << 2)
#define ASCON_CTRL_DECRYPT (1U << 3)
#define ASCON_CTRL_ENCRYPT_FINAL (1U << 4)
#define ASCON_CTRL_DECRYPT_FINAL (1U << 5)
#define ASCON_CTRL_ASSOCIATE_FINAL (1U << 6)

#define UNIQUE_VALUE 0xdeadbeef

int ascon_is_sane(void)
{
	return ASCON128_INTERFACE->unique_value == UNIQUE_VALUE;
}

int ascon_is_busy(void)
{
	return ASCON128_INTERFACE->status;
}

static inline void ascon_busy_wait(void)
{
	while (ascon_is_busy())
		;
}

static void ascon_memcpy(volatile uint32_t *restrict dest, const volatile uint32_t *restrict src, size_t size)
{
	size /= sizeof(*src);
	while (size-- > 0) {
		*dest++ = *src++;
	}
}

static void ascon_operation(struct ascon_param *param, uint32_t op, uint32_t op_final)
{
	size_t i;

	ascon_busy_wait();
	ascon_memcpy(ASCON128_INTERFACE->key, param->key, sizeof(ASCON128_INTERFACE->key));
	ASCON128_INTERFACE->control = ASCON_CTRL_INIT;

	for (i = 0; i < param->associated_size; ++i) {
		ascon_busy_wait();
		ascon_memcpy(ASCON128_INTERFACE->data, param->associated[i], sizeof(ASCON128_INTERFACE->data));
		ASCON128_INTERFACE->control = (i == param->associated_size - 1) ? ASCON_CTRL_ASSOCIATE_FINAL : ASCON_CTRL_ASSOCIATE;
	}

	for (i = 0; i < param->data_size; ++i) {
		ascon_busy_wait();
		if (i) {
			ascon_memcpy(param->data_out[i-1], ASCON128_INTERFACE->data, sizeof(param->data_in[i-1]));
		}
		ascon_memcpy(ASCON128_INTERFACE->data, param->data_in[i], sizeof(ASCON128_INTERFACE->data));
		ASCON128_INTERFACE->control = (i == param->data_size - 1) ? op_final : op;
	}
	ascon_busy_wait();
	ascon_memcpy(param->data_out[i-1], ASCON128_INTERFACE->data, sizeof(param->data_in[i-1]));
	ascon_memcpy(param->nonce, ASCON128_INTERFACE->nonce, sizeof(param->nonce));
}

void ascon_encrypt(struct ascon_param *param)
{
	ascon_operation(param, ASCON_CTRL_ENCRYPT, ASCON_CTRL_ENCRYPT_FINAL);
}

void ascon_decrypt(struct ascon_param *param)
{
	ascon_operation(param, ASCON_CTRL_DECRYPT, ASCON_CTRL_DECRYPT_FINAL);
}
