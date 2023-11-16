#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "usbd_hid.h"

/**********************
 * KEYBOARD MODIFIERS *
 **********************/
#define MOD_ALT_RIGHT   (1 << 6)
#define MOD_SHIFT_LEFT  (1 << 1)
#define MOD_NO_MODIFIER 0

typedef struct {
    uint8_t MODIFIER;
    uint8_t RESERVED;
    uint8_t KEYCODE1;
    uint8_t KEYCODE2;
    uint8_t KEYCODE3;
    uint8_t KEYCODE4;
    uint8_t KEYCODE5;
    uint8_t KEYCODE6;
} keyboardHID;

//struct to store characters and keyboard codes
typedef struct {
    uint8_t ASCII;
    uint8_t usage_id;
    uint8_t modifier;
} Keys;

extern USBD_HandleTypeDef hUsbDeviceFS;
extern keyboardHID        keyboardhid;
extern Keys               US_keys[];
extern Keys               HU_keys[];

void typeString(char *str);

#endif // __KEYBOARD_H

