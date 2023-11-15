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

extern RNG_HandleTypeDef hrng;

int8_t  pincode[DIGITS] = {1, 0, 0, 0, 0, 0};
uint8_t unlocked        = 0;

uint8_t checkPin(int8_t *pin1, int8_t *pin2)
{
    for (int i = 0; i < DIGITS; i++) {
        if (pin1[i] != pin2[i]) {
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

static inline void drawPin(int8_t *pin, uint8_t *selected) {
    setDigits(pin, selected);
    drawDigits(pin, *selected);
}

void changePin()
{
    int8_t  newpin1[DIGITS] = {0};
    int8_t  newpin2[DIGITS] = {0};
    uint8_t selectedDigit   = 0;
    uint8_t flag            = 1;

    ssd1306_Fill(Black); // clear screen
    if (!getPin()) {
        return;
    }
    while (1) {
        ssd1306_Fill(Black); // clear screen

        if (btn1()) {
            lock();
            return;
        }
        if (btn2()) {
            selectedDigit = 0;
            if (flag) {
                flag = 0;
                continue;
            }
            if (checkPin(newpin1, newpin2)) {
                memcpy(pincode, newpin1, sizeof(pincode));
                sendStatus(&Font_11x18, "New PIN set");
                lock();
                return;
            } else {
                memset(newpin1, 0, sizeof(newpin1));
                memset(newpin2, 0, sizeof(newpin2));
                flag = 1;
                sendStatus(&Font_7x10, "PINs don't match");
            }
        }
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString(flag ? "Enter new PIN:" : "Enter again:", Font_7x10, Black);
        //setDigits(flag ? newpin1 : newpin2, &selectedDigit);
        //drawDigits(flag ? newpin1 : newpin2, selectedDigit);
        drawPin(flag ? newpin1 : newpin2, &selectedDigit);
    }
}

void lock()
{
    unlocked = 0;
    LED_Off();
}

uint8_t getPin()
{
    int8_t pinFromUser[DIGITS] = {0};
    ssd1306_Fill(Black); // clear screen
    uint8_t selectedDigit = 0;
    while (1) {
        if (btn1()) {
            return 0;
        }
        if (btn2()) {
            if (checkPin(pinFromUser, pincode)) {
                unlocked       = 1;
                AUTOLOCK_TIMER = 0;
                LED_On();
                return 1;
            }
            // reset pins
            memset(pinFromUser, 0, DIGITS);
            selectedDigit = 0;
        }

        //setDigits(pinFromUser, &selectedDigit);
        //drawDigits(pinFromUser, selectedDigit);
        drawPin(pinFromUser, &selectedDigit);
    }
}

void createUser()
{
    UserData   u = {0};
    SHA256_CTX ctx;

    sha256_init(&ctx);
    //sha256_update(&ctx, (BYTE *)pw, strlen(pw));
    sha256_final(&ctx, (BYTE *)u.hashedPW);

    for (int i = 0; i < sizeof(u.keyNonce) / 4; i++) {
        HAL_RNG_GenerateRandomNumber(&hrng, &u.keyNonce[i]);
        HAL_RNG_GenerateRandomNumber(&hrng, &u.recordNonce[i]);
    }

    for (int i = 0; i < sizeof(u.recordKey) / 4; i++) {
        HAL_RNG_GenerateRandomNumber(&hrng, &u.recordKey[i]);
    }

}

