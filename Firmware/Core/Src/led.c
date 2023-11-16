#include "led.h"

void LED_On() { LED = LED_BRIGHTNESS; }

void LED_Off() { LED = 0; }

void LED_Toggle() { LED ? LED_Off() : LED_On(); }

