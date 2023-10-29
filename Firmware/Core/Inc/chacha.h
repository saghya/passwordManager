#ifndef __CHACHA_H
#define __CHACHA_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
    // uint32_t magic[4];
    uint32_t key[8];
    uint32_t counter;
    uint32_t nonce[3];
    uint32_t state[16];
    uint32_t block[16];
} chacha_context;

void chacha_init(chacha_context *ctx, uint8_t key[32], uint8_t nonce[12]);
void chacha_xor(chacha_context *ctx, uint8_t *data, ssize_t len);

#endif // __CHACHA_H

