#ifndef __LED_H
#define __LED_H
#include "main.h"

#define LED_BRIGHTNESS 1000
#define LED            TIM15->CCR2

void LED_On();
void LED_Off();
void LED_Toggle();

#endif // __LED_H

