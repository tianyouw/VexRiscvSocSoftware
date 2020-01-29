#ifndef ASCON_H
#define ASCON_H

#include <stddef.h>
#include <stdint.h>

#define BITS_TO_UINT32(b) ((b) / (sizeof(uint32_t) * 8))

typedef uint32_t ascon_key_t[BITS_TO_UINT32(128)];
typedef uint32_t ascon_nonce_t[BITS_TO_UINT32(128)];

struct ascon_param {
	ascon_key_t key;
	const uint32_t *(associated[4]);
	size_t associated_size;
	const uint32_t *(data_in[4]);
	size_t data_size;
	uint32_t *(data_out[4]);
	ascon_nonce_t nonce;
};

int ascon_is_sane(void);
void ascon_encrypt(struct ascon_param *param);
void ascon_decrypt(struct ascon_param *param);

#endif /* define ASCON_H */
