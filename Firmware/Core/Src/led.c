#include "led.h"

void LED_On() {
	LED = LED_BRIGHTNESS;
}

void LED_Off() {
	LED = 0;
}

void toggleLED() {
	if (LED) {
		LED_Off();
	} else {
		LED_On();
	}
}
