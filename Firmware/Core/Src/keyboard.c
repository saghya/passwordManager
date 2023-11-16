#include "keyboard.h"
#include "pin.h"
#include "userdata.h"

#define USB_DELAY 20

keyboardHID keyboardhid = {0};

Keys US_keys[] = {
    { '!',  0x1E, MOD_SHIFT_LEFT },
    { '"',  0x34, MOD_SHIFT_LEFT },
    { '#',  0x20, MOD_SHIFT_LEFT },
    { '$',  0x21, MOD_SHIFT_LEFT },
    { '%',  0x22, MOD_SHIFT_LEFT },
    { '&',  0x24, MOD_SHIFT_LEFT },
    { '\'', 0x34, MOD_NO_MODIFIER },
    { '(',  0x26, MOD_SHIFT_LEFT },
    { ')',  0x27, MOD_SHIFT_LEFT },
    { '*',  0x25, MOD_SHIFT_LEFT },
    { '+',  0x2E, MOD_SHIFT_LEFT },
    { ',',  0x36, MOD_NO_MODIFIER },
    { '-',  0x2D, MOD_NO_MODIFIER },
    { '.',  0x37, MOD_NO_MODIFIER },
    { '/',  0x38, MOD_NO_MODIFIER },
    { '0',  0x27, MOD_NO_MODIFIER },
    { '1',  0x1E, MOD_NO_MODIFIER },
    { '2',  0x1F, MOD_NO_MODIFIER },
    { '3',  0x20, MOD_NO_MODIFIER },
    { '4',  0x21, MOD_NO_MODIFIER },
    { '5',  0x22, MOD_NO_MODIFIER },
    { '6',  0x23, MOD_NO_MODIFIER },
    { '7',  0x24, MOD_NO_MODIFIER },
    { '8',  0x25, MOD_NO_MODIFIER },
    { '9',  0x26, MOD_NO_MODIFIER },
    { ':',  0x33, MOD_SHIFT_LEFT },
    { ';',  0x33, MOD_NO_MODIFIER },
    { '<',  0x36, MOD_SHIFT_LEFT },
    { '=',  0x2E, MOD_NO_MODIFIER },
    { '>',  0x37, MOD_SHIFT_LEFT },
    { '?',  0x38, MOD_SHIFT_LEFT },
    { '@',  0x1F, MOD_SHIFT_LEFT },
    { 'A',  0x04, MOD_SHIFT_LEFT },
    { 'B',  0x05, MOD_SHIFT_LEFT },
    { 'C',  0x06, MOD_SHIFT_LEFT },
    { 'D',  0x07, MOD_SHIFT_LEFT },
    { 'E',  0x08, MOD_SHIFT_LEFT },
    { 'F',  0x09, MOD_SHIFT_LEFT },
    { 'G',  0x0A, MOD_SHIFT_LEFT },
    { 'H',  0x0B, MOD_SHIFT_LEFT },
    { 'I',  0x0C, MOD_SHIFT_LEFT },
    { 'J',  0x0D, MOD_SHIFT_LEFT },
    { 'K',  0x0E, MOD_SHIFT_LEFT },
    { 'L',  0x0F, MOD_SHIFT_LEFT },
    { 'M',  0x10, MOD_SHIFT_LEFT },
    { 'N',  0x11, MOD_SHIFT_LEFT },
    { 'O',  0x12, MOD_SHIFT_LEFT },
    { 'P',  0x13, MOD_SHIFT_LEFT },
    { 'Q',  0x14, MOD_SHIFT_LEFT },
    { 'R',  0x15, MOD_SHIFT_LEFT },
    { 'S',  0x16, MOD_SHIFT_LEFT },
    { 'T',  0x17, MOD_SHIFT_LEFT },
    { 'U',  0x18, MOD_SHIFT_LEFT },
    { 'V',  0x19, MOD_SHIFT_LEFT },
    { 'W',  0x1A, MOD_SHIFT_LEFT },
    { 'X',  0x1B, MOD_SHIFT_LEFT },
    { 'Y',  0x1C, MOD_SHIFT_LEFT },
    { 'Z',  0x1D, MOD_SHIFT_LEFT },
    { '[',  0x2F, MOD_NO_MODIFIER },
    { '\\', 0x31, MOD_NO_MODIFIER },
    { ']',  0x30, MOD_NO_MODIFIER },
    { '^',  0x23, MOD_SHIFT_LEFT },
    { '_',  0x2D, MOD_SHIFT_LEFT },
    { '`',  0x35, MOD_NO_MODIFIER },
    { 'a',  0x04, MOD_NO_MODIFIER },
    { 'b',  0x05, MOD_NO_MODIFIER },
    { 'c',  0x06, MOD_NO_MODIFIER },
    { 'd',  0x07, MOD_NO_MODIFIER },
    { 'e',  0x08, MOD_NO_MODIFIER },
    { 'f',  0x09, MOD_NO_MODIFIER },
    { 'g',  0x0A, MOD_NO_MODIFIER },
    { 'h',  0x0B, MOD_NO_MODIFIER },
    { 'i',  0x0C, MOD_NO_MODIFIER },
    { 'j',  0x0D, MOD_NO_MODIFIER },
    { 'k',  0x0E, MOD_NO_MODIFIER },
    { 'l',  0x0F, MOD_NO_MODIFIER },
    { 'm',  0x10, MOD_NO_MODIFIER },
    { 'n',  0x11, MOD_NO_MODIFIER },
    { 'o',  0x12, MOD_NO_MODIFIER },
    { 'p',  0x13, MOD_NO_MODIFIER },
    { 'q',  0x14, MOD_NO_MODIFIER },
    { 'r',  0x15, MOD_NO_MODIFIER },
    { 's',  0x16, MOD_NO_MODIFIER },
    { 't',  0x17, MOD_NO_MODIFIER },
    { 'u',  0x18, MOD_NO_MODIFIER },
    { 'v',  0x19, MOD_NO_MODIFIER },
    { 'w',  0x1A, MOD_NO_MODIFIER },
    { 'x',  0x1B, MOD_NO_MODIFIER },
    { 'y',  0x1D, MOD_NO_MODIFIER },
    { 'z',  0x1C, MOD_NO_MODIFIER },
    { '{',  0x2F, MOD_SHIFT_LEFT },
    { '|',  0x31, MOD_SHIFT_LEFT },
    { '}',  0x30, MOD_SHIFT_LEFT },
    { '~',  0x35, MOD_SHIFT_LEFT },
    { '\t', 0x2b, MOD_NO_MODIFIER },
    { '\n', 0x28, MOD_NO_MODIFIER },
    { '\0', 0x00, MOD_NO_MODIFIER },
};

Keys HU_keys[] = {
    { '0',  0x35, MOD_NO_MODIFIER },
    { '1',  0x1E, MOD_NO_MODIFIER },
    { '2',  0x1F, MOD_NO_MODIFIER },
    { '3',  0x20, MOD_NO_MODIFIER },
    { '4',  0x21, MOD_NO_MODIFIER },
    { '5',  0x22, MOD_NO_MODIFIER },
    { '6',  0x23, MOD_NO_MODIFIER },
    { '7',  0x24, MOD_NO_MODIFIER },
    { '8',  0x25, MOD_NO_MODIFIER },
    { '9',  0x26, MOD_NO_MODIFIER },
    { 0xb6, 0x27, MOD_NO_MODIFIER }, //ö
    { 0xbc, 0x2D, MOD_NO_MODIFIER }, //ü
    { 0xb3, 0x2E, MOD_NO_MODIFIER }, //ó
    { 0xa7, 0x35, MOD_SHIFT_LEFT },  //§
    { '\'', 0x1E, MOD_SHIFT_LEFT },
    { '"',  0x1F, MOD_SHIFT_LEFT },
    { '+',  0x20, MOD_SHIFT_LEFT },
    { '!',  0x21, MOD_SHIFT_LEFT },
    { '%',  0x22, MOD_SHIFT_LEFT },
    { '/',  0x23, MOD_SHIFT_LEFT },
    { '=',  0x24, MOD_SHIFT_LEFT },
    { '(',  0x25, MOD_SHIFT_LEFT },
    { ')',  0x26, MOD_SHIFT_LEFT },
    { 0x96, 0x27, MOD_SHIFT_LEFT },  //Ö
    { 0x9c, 0x2D, MOD_SHIFT_LEFT }, //Ü
    { 0x93, 0x2E, MOD_SHIFT_LEFT }, //Ó
    { '~',  0x1E, MOD_ALT_RIGHT },
    { '^',  0x20, MOD_ALT_RIGHT },
    { '`',  0x24, MOD_ALT_RIGHT },
    { '\t', 0x2B, MOD_NO_MODIFIER },
    { '\n', 0x28, MOD_NO_MODIFIER },
    { ' ',  0x2C, MOD_NO_MODIFIER },
    { 'a',  0x04, MOD_NO_MODIFIER },
    { 's',  0x16, MOD_NO_MODIFIER },
    { 'd',  0x07, MOD_NO_MODIFIER },
    { 'f',  0x09, MOD_NO_MODIFIER },
    { 'g',  0x0A, MOD_NO_MODIFIER },
    { 'h',  0x0B, MOD_NO_MODIFIER },
    { 'j',  0x0D, MOD_NO_MODIFIER },
    { 'k',  0x0E, MOD_NO_MODIFIER },
    { 'l',  0x0F, MOD_NO_MODIFIER },
    { 0xa9, 0x33, MOD_NO_MODIFIER }, //é
    { 0xa1, 0x34, MOD_NO_MODIFIER }, //á
    { 'A',  0x04, MOD_SHIFT_LEFT },
    { 'S',  0x16, MOD_SHIFT_LEFT },
    { 'D',  0x07, MOD_SHIFT_LEFT },
    { 'F',  0x09, MOD_SHIFT_LEFT },
    { 'G',  0x0A, MOD_SHIFT_LEFT },
    { 'H',  0x0B, MOD_SHIFT_LEFT },
    { 'J',  0x0D, MOD_SHIFT_LEFT },
    { 'K',  0x0E, MOD_SHIFT_LEFT },
    { 'L',  0x0F, MOD_SHIFT_LEFT },
    { 0x89, 0x33, MOD_SHIFT_LEFT }, //É
    { 0x81, 0x34, MOD_SHIFT_LEFT }, //Á
    { '[',  0x09, MOD_ALT_RIGHT },
    { ']',  0x0A, MOD_ALT_RIGHT },
    { '$',  0x33, MOD_ALT_RIGHT },
    { 0x9f, 0x34, MOD_ALT_RIGHT }, //ß
    { 'q',  0x14, MOD_NO_MODIFIER },
    { 'w',  0x1A, MOD_NO_MODIFIER },
    { 'e',  0x08, MOD_NO_MODIFIER },
    { 'r',  0x15, MOD_NO_MODIFIER },
    { 't',  0x17, MOD_NO_MODIFIER },
    { 'z',  0x1C, MOD_NO_MODIFIER },
    { 'u',  0x18, MOD_NO_MODIFIER },
    { 'i',  0x0C, MOD_NO_MODIFIER },
    { 'o',  0x12, MOD_NO_MODIFIER },
    { 'p',  0x13, MOD_NO_MODIFIER },
    { 0xf5, 0x2F, MOD_NO_MODIFIER }, //ő
    { 0xba, 0x30, MOD_NO_MODIFIER }, //ú
    { 0xfb, 0x31, MOD_NO_MODIFIER }, //ű
    { 'Q',  0x14, MOD_SHIFT_LEFT },
    { 'W',  0x1A, MOD_SHIFT_LEFT },
    { 'E',  0x08, MOD_SHIFT_LEFT },
    { 'R',  0x15, MOD_SHIFT_LEFT },
    { 'T',  0x17, MOD_SHIFT_LEFT },
    { 'Z',  0x1C, MOD_SHIFT_LEFT },
    { 'U',  0x18, MOD_SHIFT_LEFT },
    { 'I',  0x0C, MOD_SHIFT_LEFT },
    { 'O',  0x12, MOD_SHIFT_LEFT },
    { 'P',  0x13, MOD_SHIFT_LEFT },
    { 0xd5, 0x2F, MOD_SHIFT_LEFT }, //Ő
    { 0x9a, 0x30, MOD_SHIFT_LEFT }, //Ú
    { 0xdb, 0x31, MOD_SHIFT_LEFT }, //Ű
    { '\\', 0x14, MOD_ALT_RIGHT },
    { '|',  0x1A, MOD_ALT_RIGHT },
    { 0x84, 0x08, MOD_ALT_RIGHT },  //Ä
    { 0xac, 0x18, MOD_ALT_RIGHT }, //€
    { 0xb7, 0x2F, MOD_ALT_RIGHT }, //÷
    { 0x97, 0x30, MOD_ALT_RIGHT }, //×
    { 'y',  0x1D, MOD_NO_MODIFIER },
    { 'x',  0x1B, MOD_NO_MODIFIER },
    { 'c',  0x06, MOD_NO_MODIFIER },
    { 'v',  0x19, MOD_NO_MODIFIER },
    { 'b',  0x05, MOD_NO_MODIFIER },
    { 'n',  0x11, MOD_NO_MODIFIER },
    { 'm',  0x10, MOD_NO_MODIFIER },
    { ',',  0x36, MOD_NO_MODIFIER },
    { '.',  0x37, MOD_NO_MODIFIER },
    { '-',  0x38, MOD_NO_MODIFIER },
    { 'Y',  0x1D, MOD_SHIFT_LEFT },
    { 'X',  0x1B, MOD_SHIFT_LEFT },
    { 'C',  0x06, MOD_SHIFT_LEFT },
    { 'V',  0x19, MOD_SHIFT_LEFT },
    { 'B',  0x05, MOD_SHIFT_LEFT },
    { 'N',  0x11, MOD_SHIFT_LEFT },
    { 'M',  0x10, MOD_SHIFT_LEFT },
    { '?',  0x36, MOD_SHIFT_LEFT },
    { ':',  0x37, MOD_SHIFT_LEFT },
    { '_',  0x38, MOD_SHIFT_LEFT },
    { '>',  0x1D, MOD_ALT_RIGHT },
    { '#',  0x1B, MOD_ALT_RIGHT },
    { '&',  0x06, MOD_ALT_RIGHT },
    { '@',  0x19, MOD_ALT_RIGHT },
    { '{',  0x05, MOD_ALT_RIGHT },
    { '}',  0x11, MOD_ALT_RIGHT },
    { '<',  0x10, MOD_ALT_RIGHT },
    { ';',  0x36, MOD_ALT_RIGHT },
    { '>',  0x37, MOD_ALT_RIGHT },
    { '*',  0x38, MOD_ALT_RIGHT },
    { 0xad, 0x64, MOD_NO_MODIFIER }, //í
    { 0x8d, 0x64, MOD_SHIFT_LEFT }, //Í
    { '<',  0x64, MOD_ALT_RIGHT },
    { '\0', 0x00, MOD_NO_MODIFIER }
};

void write(uint8_t key, uint8_t modifier)
{
    // key down
    keyboardhid.KEYCODE1 = key;
    keyboardhid.MODIFIER = modifier;
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&keyboardhid,
                        sizeof(keyboardhid));
    HAL_Delay(USB_DELAY);

    // key up
    keyboardhid.MODIFIER = 0x00;
    keyboardhid.KEYCODE1 = 0x00;
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&keyboardhid,
                        sizeof(keyboardhid));
    HAL_Delay(USB_DELAY);
}

void typeString(char *data)
{
    Keys *keys = ((UserData*)USER_DATA_ADDR)->keyboard;
    for (int i = 0; data[i] != '\0'; i++) {
        for (uint8_t k = 0; keys[k].ASCII != 0; k++) {
            if (data[i] == keys[k].ASCII) {
                write(keys[k].usage_id, keys[k].modifier);
                //for altgr + keys we have to press the button twice
                if (data[i] == keys[k + 1].ASCII) {
                    write(keys[k + 1].usage_id, keys[k + 1].modifier);
                }
                break;
            }
        }
    }
}

