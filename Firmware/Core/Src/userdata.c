#include "userdata.h"
#include "chacha.h"
#include "sha256.h"
#include "pin.h"
#include "screen.h"
#include "buttons.h"
#include <stdint.h>

extern RNG_HandleTypeDef hrng;

uint8_t saveUserData(UserData *u)
{
    uint8_t        key[32] = {0};
    chacha_context chacha_ctx;
    SHA256_CTX     sha_ctx;

    sha256_init(&sha_ctx);
    sha256_update(&sha_ctx, (BYTE *)pin, DIGITS);
    sha256_final(&sha_ctx, (BYTE *)u->hashedPW);

    memcpy(key, pin, DIGITS);
    chacha_init(&chacha_ctx, key, (uint8_t *)u->keyNonce);
    chacha_xor(&chacha_ctx, (uint8_t *)u->recordKey, sizeof(u->recordKey));
    chacha_xor(&chacha_ctx, (uint8_t *)u->recordNonce, sizeof(u->recordNonce));

    Flash_Write_Data(USER_DATA_ADDR, (uint64_t *)u, sizeof(UserData) / 8);
    return 1;
}

void createUser()
{
    UserData   u = {0};
    SHA256_CTX ctx;

    while (!setPin()) {
        sendStatus(&Font_7x10, "PIN is required");
    }

    sha256_init(&ctx);
    sha256_update(&ctx, (BYTE *)pin, DIGITS);
    sha256_final(&ctx, (BYTE *)u.hashedPW);

    for (int i = 0; i < sizeof(u.keyNonce) / 4; i++) {
        HAL_RNG_GenerateRandomNumber(&hrng, &u.keyNonce[i]);
        HAL_RNG_GenerateRandomNumber(&hrng, &u.recordNonce[i]);
    }

    for (int i = 0; i < sizeof(u.recordKey) / 4; i++) {
        HAL_RNG_GenerateRandomNumber(&hrng, &u.recordKey[i]);
    }
    u.keyboard = HU_keys;

    saveUserData(&u);
}

uint8_t readUserData(uint32_t addr, UserData *u)
{
    uint8_t        key[32] = {0};
    chacha_context ctx;

    if (!unlock())
        return 0;
    memcpy(u, (uint8_t*)USER_DATA_ADDR, sizeof(UserData));

    memcpy(key, pin, DIGITS);
    chacha_init(&ctx, key, (uint8_t *)u->keyNonce);
    chacha_xor(&ctx, (uint8_t *)u->recordKey, sizeof(u->recordKey));
    chacha_xor(&ctx, (uint8_t *)u->recordNonce, sizeof(u->recordNonce));
    return 1;
}

