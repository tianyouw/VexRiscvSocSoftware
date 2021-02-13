#ifndef ASCON_H
#define ASCON_H

#include <stddef.h>
#include <stdint.h>

#define BITS_TO_UINT32(b) ((b) / (sizeof(uint32_t) * 8))
#define WIDE_LEN BITS_TO_UINT32(128)

typedef uint32_t ascon_key_t[WIDE_LEN];
typedef uint32_t ascon_nonce_t[WIDE_LEN];
typedef uint32_t ascon_tag_t[WIDE_LEN];
typedef uint32_t ascon_data_block_t[WIDE_LEN];

struct ascon_param {
	ascon_key_t key_in;
	ascon_nonce_t nonce_in;
	ascon_data_block_t *associated;
	size_t associated_size;
	ascon_data_block_t *data_in;
	size_t data_size;
	ascon_data_block_t *data_out;
	ascon_tag_t tag_out;
};

int ascon_is_sane(void);
void ascon_encrypt(struct ascon_param *param);
void ascon_decrypt(struct ascon_param *param);

#endif /* define ASCON_H */
