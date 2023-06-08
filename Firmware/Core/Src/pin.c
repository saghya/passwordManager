#include <stdlib.h>
#include <string.h>
#include "pin.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "encoder.h"
#include "buttons.h"
#include "led.h"

int8_t pincode[DIGITS] = { 1, 0, 0, 0, 0, 0 };
uint8_t unlocked = 0;

uint8_t checkPin(int8_t *pin1, int8_t *pin2) {
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

	pin[*selected] += -encoderDelta();
	// handle overflow
	if (pin[*selected] > 9) {
		pin[*selected] = 0;
	} else if (pin[*selected] < 0) {
		pin[*selected] = 9;
	}
}

void drawDigits(int8_t *pin, uint8_t selected) {

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

void changePin()
{
	int8_t newpin1[DIGITS] = {0};
	int8_t newpin2[DIGITS] = {0};
	uint8_t selectedDigit = 0;
	uint8_t flag = 1;

	ssd1306_Fill(Black); // clear screen
	if (!unlock())
	{
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
			ssd1306_SetCursor(0, (SSD1306_HEIGHT - Font_11x18.height) / 2); // center
			if (checkPin(newpin1, newpin2)) {
				memcpy(pincode, newpin1, sizeof(pincode));
				ssd1306_WriteString("New PIN set", Font_11x18, Black);
				ssd1306_UpdateScreen();
				HAL_Delay(1000);
				lock();
				return;
			} else {
				memset(newpin1, 0, sizeof(newpin1));
				memset(newpin2, 0, sizeof(newpin2));
				flag = 1;
				ssd1306_WriteString("PINs don't match", Font_7x10, Black);
				ssd1306_UpdateScreen();
				HAL_Delay(1000);
			}
		}
		ssd1306_SetCursor(0, 0);
		ssd1306_WriteString(flag ? "Enter new PIN:" : "Enter again:", Font_7x10, Black);
		setDigits(flag ? newpin1 : newpin2, &selectedDigit);
		drawDigits(flag ? newpin1 : newpin2, selectedDigit);
	}
}

void lock() {
	unlocked = 0;
	LED_Off();
}

uint8_t unlock() {
	int8_t pinFromUser[DIGITS] = {0};
	ssd1306_Fill(Black); // clear screen
	uint8_t selectedDigit = 0;
	while (1) {
		if (btn1()) {
			return 0;
		}
		if (btn2()) {
			if (checkPin(pinFromUser, pincode)) {
				unlocked = 1;
				AUTOLOCK_TIMER = 0;
				LED_On();
				return 1;
			}
			// reset pins
			memset(pinFromUser, 0, DIGITS);
			selectedDigit = 0;
		}

		setDigits(pinFromUser, &selectedDigit);
		drawDigits(pinFromUser, selectedDigit);
	}
}
