#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "flash.h"
#include "screen.h"
#include "pin.h"
#include "encoder.h"
#include "buttons.h"
#include "led.h"
#include "sha256.h"
#include "stm32l4xx_hal_rng.h"
#include "chacha.h"

extern RNG_HandleTypeDef hrng;

int8_t pin[DIGITS] = {0};

void saveUserData(UserData *u);

uint8_t checkPin()
{
    uint8_t    buff[32] = {0};
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (BYTE *)pin, DIGITS);
    sha256_final(&ctx, (BYTE *)buff);
    for (int i = 0; i < sizeof(buff); i++) {
        if (buff[i] != ((UserData *)USER_DATA_ADDR)->hashedPW[i]) {
            return 0;
        }
    }
    return 1;
}

void setDigits(int8_t *pin, uint8_t *selected)
{
    if (e_sw()) {
        *selected = (*selected + 1) % DIGITS;
    }

    pin[*selected] += encoderDelta();
    // handle overflow
    if (pin[*selected] > 9) {
        pin[*selected] = 0;
    } else if (pin[*selected] < 0) {
        pin[*selected] = 9;
    }
}

void drawDigits(int8_t *pin, uint8_t selected)
{
    FontDef digitFont = Font_16x24;
    uint8_t x, y;
    for (int i = 0; i < DIGITS; i++) {
        x = i * SSD1306_WIDTH / (DIGITS + 1) + SSD1306_WIDTH / DIGITS / 2;
        y = (SSD1306_HEIGHT - digitFont.height) / 2;
        ssd1306_SetCursor(x, y);
        //ssd1306_WriteChar('0' + pin[i], digitFont, i == selected ? Black : White);
        ssd1306_WriteChar(i == selected ? '0' + pin[i] : '*', digitFont, i == selected ? Black : White);
    }

    ssd1306_UpdateScreen();
}

static inline void drawPin(int8_t *pin, uint8_t *selected)
{
    setDigits(pin, selected);
    drawDigits(pin, *selected);
}

uint8_t getPin()
{
    ssd1306_Fill(Black); // clear screen
    uint8_t selectedDigit = 0;
    while (1) {
        if (btn1()) {
            return 0;
        }
        if (btn2()) {
            return 1;
        }
        drawPin(pin, &selectedDigit);
    }
}

uint8_t unlock()
{
    while (!checkPin()) {
        if (btn1()) {
            return 0;
        }
        getPin();
    }
    return 1;
}

uint8_t setPin()
{
    int8_t  newpin1[DIGITS] = {0};
    int8_t  newpin2[DIGITS] = {0};
    uint8_t selectedDigit   = 0;
    uint8_t flag            = 1;

    while (1) {
        ssd1306_Fill(Black); // clear screen

        if (btn1()) {
            return 0;
        }
        if (btn2()) {
            selectedDigit = 0;
            if (flag) {
                flag = 0;
                continue;
            }
            if (!strncmp((char *)newpin1, (char *)newpin2, DIGITS)) {
                memcpy(pin, newpin1, DIGITS);
                sendStatus(&Font_11x18, "New PIN set");
                return 1;
            } else {
                memset(newpin1, 0, sizeof(newpin1));
                memset(newpin2, 0, sizeof(newpin2));
                flag = 1;
                sendStatus(&Font_7x10, "PINs don't match");
            }
        }
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString(flag ? "Enter new PIN:" : "Enter again:", Font_7x10, Black);
        drawPin(flag ? newpin1 : newpin2, &selectedDigit);
    }
}

uint8_t changePin()
{
    UserData u = {0};
    if (!unlock()) {
        return 0;
    }
    readUserData(USER_DATA_ADDR, &u);
    if (setPin()) {
        saveUserData(&u);
        return 1;
    }
    return 0;
}

void lock()
{
    memset(pin, 0, DIGITS);
    LED_Off();
}

void saveUserData(UserData *u)
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

    saveUserData(&u);
}

void readUserData(uint32_t addr, UserData *u)
{
    uint8_t        key[32] = {0};
    chacha_context ctx;

    memcpy(u, (uint8_t*)USER_DATA_ADDR, sizeof(UserData));
    while (!unlock()) {
        if (btn1()) {
            return;
        }
    }

    memcpy(key, pin, DIGITS);
    chacha_init(&ctx, key, (uint8_t *)u->keyNonce);
    chacha_xor(&ctx, (uint8_t *)u->recordKey, sizeof(u->recordKey));
    chacha_xor(&ctx, (uint8_t *)u->recordNonce, sizeof(u->recordNonce));
}

