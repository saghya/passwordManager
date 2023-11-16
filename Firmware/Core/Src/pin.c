#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "pin.h"
#include "encoder.h"
#include "buttons.h"
#include "led.h"
#include "sha256.h"
#include "chacha.h"
#include "userdata.h"
#include "record.h"

int8_t pin[DIGITS] = {0};

static inline void setDigits(int8_t *pin, uint8_t *selected)
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

static inline void drawDigits(int8_t *pin, uint8_t selected)
{
    FontDef digitFont = Font_16x24;
    uint8_t x, y;
    for (int i = 0; i < DIGITS; i++) {
        x = i * SSD1306_WIDTH / (DIGITS + 1) + SSD1306_WIDTH / DIGITS / 2;
        y = (SSD1306_HEIGHT - digitFont.height) / 2;
        ssd1306_SetCursor(x, y);
        ssd1306_WriteChar(i == selected ? '0' + pin[i] : '*', digitFont, i == selected ? Black : White);
    }

    ssd1306_UpdateScreen();
}

static inline void drawPin(int8_t *pin, uint8_t *selected)
{
    setDigits(pin, selected);
    drawDigits(pin, *selected);
}

uint8_t PIN_Get()
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

uint8_t PIN_Set()
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
                Screen_SendStatus(&Font_11x18, "New PIN set");
                return 1;
            } else {
                memset(newpin1, 0, sizeof(newpin1));
                memset(newpin2, 0, sizeof(newpin2));
                flag = 1;
                Screen_SendStatus(&Font_7x10, "PINs don't match");
            }
        }
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString(flag ? "Enter new PIN:" : "Enter again:", Font_7x10, Black);
        drawPin(flag ? newpin1 : newpin2, &selectedDigit);
    }
}

uint8_t PIN_Check()
{
    uint8_t    buff[32] = {0};
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (BYTE *)pin, DIGITS);
    sha256_final(&ctx, (BYTE *)buff);
    for (int i = 0; i < sizeof(buff); i++) {
        if (buff[i] != ((UserData *)USER_DATA_ADDR)->hashedPW[i]) {
            memset(pin, 0, DIGITS);
            return 0;
        }
    }
    return 1;
}

uint8_t PIN_Change()
{
    UserData u = {0};
    if (!UserData_Read(USER_DATA_ADDR, &u))
        return 0;
    if (!PIN_Set()) {
        return 1;
    }
    if (!UserData_Save(&u))
        return 0;
    return 1;
}

uint8_t unlock()
{
    while (!PIN_Check()) {
        if (btn1()) {
            return 0;
        }
        PIN_Get();
    }
    LED_On();
    return 1;
}

void lock()
{
    memset(pin, 0, DIGITS);
    LED_Off();
}

