#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "chacha.h"

#define ROTL(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
#define QR(a, b, c, d) (			\
	a += b,  d ^= a,  d = ROTL(d,16),	\
	c += d,  b ^= c,  b = ROTL(b,12),	\
	a += b,  d ^= a,  d = ROTL(d, 8),	\
	c += d,  b ^= c,  b = ROTL(b, 7))
#define ROUNDS 20

static inline void chacha_write_to_state(chacha_context *ctx, uint8_t* data, ssize_t len, uint8_t pos)
{
    uint32_t buf = 0;
    for (int i = 0; i < len; i++) {
        buf |= data[i] << (i % 4) * 8;
        if (i % 4 == 3) {
            ctx->state[pos + i / 4] = buf;
            buf = 0;
        }
    }
}
 
static inline void chacha_set_counter(chacha_context *ctx, uint32_t counter)
{
    ctx->counter = counter;
    ctx->state[12] = ctx->counter;
}

static void chacha_block(chacha_context *ctx)
{
	int i;
	uint32_t x[16];

	for (i = 0; i < 16; ++i)	
		x[i] = ctx->state[i];
	// 10 loops × 2 rounds/loop = 20 rounds
	for (i = 0; i < ROUNDS; i += 2) {
		// Odd round
		QR(x[0], x[4], x[ 8], x[12]); // column 0
		QR(x[1], x[5], x[ 9], x[13]); // column 1
		QR(x[2], x[6], x[10], x[14]); // column 2
		QR(x[3], x[7], x[11], x[15]); // column 3
		// Even round
		QR(x[0], x[5], x[10], x[15]); // diagonal 1 (main diagonal)
		QR(x[1], x[6], x[11], x[12]); // diagonal 2
		QR(x[2], x[7], x[ 8], x[13]); // diagonal 3
		QR(x[3], x[4], x[ 9], x[14]); // diagonal 4
	}
	for (i = 0; i < 16; ++i)
		ctx->block[i] = x[i] + ctx->state[i];
}

void chacha_init(chacha_context *ctx, uint8_t key[32], uint8_t nonce[12])
{
    uint8_t magic[] = "expand 32-byte k";

    memcpy(ctx->key, key, 32);
    ctx->counter = 0;
    memcpy(ctx->nonce, nonce, 12);

    memcpy(ctx->state, magic, 4*4);
    chacha_write_to_state(ctx, key, 32, 4);
    ctx->state[12] = ctx->counter;
    chacha_write_to_state(ctx, nonce, 12, 13);
}

void chacha_xor(chacha_context *ctx, uint8_t *data, ssize_t len)
{
    for (ssize_t i = 0; i < len; i++) {
        if (i % 64 == 0) {
            chacha_set_counter(ctx, ctx->counter + 1);
            chacha_block(ctx);
        }
        data[i] ^= ((uint8_t*)ctx->block)[i % 64];
    }
    chacha_set_counter(ctx, 0);
}

