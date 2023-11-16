#include "main.h"
#include "buttons.h"
#include "pin.h"

uint8_t button(uint8_t btn, uint8_t *isPressed)
{
    if (!*isPressed && !btn) {
        *isPressed     = 1;
        AUTOLOCK_TIMER = 0; // reset idle
        return 1;
    } else if (btn) {
        *isPressed = 0;
    }
    return 0;
}

uint8_t btn1()
{
    static uint8_t isPressed = 0;
    uint8_t btn = HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin);

    return button(btn, &isPressed);
}

uint8_t btn2()
{
    static uint8_t isPressed = 0;
    uint8_t btn = HAL_GPIO_ReadPin(BTN2_GPIO_Port, BTN2_Pin);

    return button(btn, &isPressed);
}

uint8_t e_sw()
{
    static uint8_t isPressed = 0;
    uint8_t btn = HAL_GPIO_ReadPin(E_SW_GPIO_Port, E_SW_Pin);

    return button(btn, &isPressed);
}

