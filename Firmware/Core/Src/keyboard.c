#include "keyboard.h"

#define USB_DELAY 15

keyboardHID keyboardhid = {0};

Keys HUN_keys[NumberOfKeys] = {
    {'0', 53, MOD_NO_MODIFIER},  {'1', 30, MOD_NO_MODIFIER},
    {'2', 31, MOD_NO_MODIFIER},  {'3', 32, MOD_NO_MODIFIER},
    {'4', 33, MOD_NO_MODIFIER},  {'5', 34, MOD_NO_MODIFIER},
    {'6', 35, MOD_NO_MODIFIER},  {'7', 36, MOD_NO_MODIFIER},
    {'8', 37, MOD_NO_MODIFIER},  {'9', 38, MOD_NO_MODIFIER},
    {182, 39, MOD_NO_MODIFIER}, //ö
    {188, 45, MOD_NO_MODIFIER}, //ü
    {179, 46, MOD_NO_MODIFIER}, //ó
    {0xa7, 53, MOD_SHIFT_LEFT}, //§
    {'\'', 30, MOD_SHIFT_LEFT},  {'"', 31, MOD_SHIFT_LEFT},
    {'+', 32, MOD_SHIFT_LEFT},   {'!', 33, MOD_SHIFT_LEFT},
    {'%', 34, MOD_SHIFT_LEFT},   {'/', 35, MOD_SHIFT_LEFT},
    {'=', 36, MOD_SHIFT_LEFT},   {'(', 37, MOD_SHIFT_LEFT},
    {')', 38, MOD_SHIFT_LEFT},   {150, 39, MOD_SHIFT_LEFT}, //Ö
    {156, 45, MOD_SHIFT_LEFT},                              //Ü
    {147, 46, MOD_SHIFT_LEFT},                              //Ó
    {'~', 30, MOD_ALT_RIGHT},    {'^', 32, MOD_ALT_RIGHT},
    {'`', 36, MOD_ALT_RIGHT},    {'\t', 43, MOD_NO_MODIFIER},
    {'\n', 40, MOD_NO_MODIFIER}, {' ', 44, MOD_NO_MODIFIER},
    {'a', 4, MOD_NO_MODIFIER},   {'s', 22, MOD_NO_MODIFIER},
    {'d', 7, MOD_NO_MODIFIER},   {'f', 9, MOD_NO_MODIFIER},
    {'g', 10, MOD_NO_MODIFIER},  {'h', 11, MOD_NO_MODIFIER},
    {'j', 13, MOD_NO_MODIFIER},  {'k', 14, MOD_NO_MODIFIER},
    {'l', 15, MOD_NO_MODIFIER},  {169, 51, MOD_NO_MODIFIER}, //é
    {0xa1, 52, MOD_NO_MODIFIER},                             //á
    {'A', 4, MOD_SHIFT_LEFT},    {'S', 22, MOD_SHIFT_LEFT},
    {'D', 7, MOD_SHIFT_LEFT},    {'F', 9, MOD_SHIFT_LEFT},
    {'G', 10, MOD_SHIFT_LEFT},   {'H', 11, MOD_SHIFT_LEFT},
    {'J', 13, MOD_SHIFT_LEFT},   {'K', 14, MOD_SHIFT_LEFT},
    {'L', 15, MOD_SHIFT_LEFT},   {137, 51, MOD_SHIFT_LEFT}, //É
    {129, 52, MOD_SHIFT_LEFT},                              //Á
    {'[', 9, MOD_ALT_RIGHT},     {']', 10, MOD_ALT_RIGHT},
    {'$', 51, MOD_ALT_RIGHT},    {159, 52, MOD_ALT_RIGHT}, //ß
    {'q', 20, MOD_NO_MODIFIER},  {'w', 26, MOD_NO_MODIFIER},
    {'e', 8, MOD_NO_MODIFIER},   {'r', 21, MOD_NO_MODIFIER},
    {'t', 23, MOD_NO_MODIFIER},  {'z', 28, MOD_NO_MODIFIER},
    {'u', 24, MOD_NO_MODIFIER},  {'i', 12, MOD_NO_MODIFIER},
    {'o', 18, MOD_NO_MODIFIER},  {'p', 19, MOD_NO_MODIFIER},
    {245, 47, MOD_NO_MODIFIER}, //ő
    {186, 48, MOD_NO_MODIFIER}, //ú
    {251, 49, MOD_NO_MODIFIER}, //ű
    {'Q', 20, MOD_SHIFT_LEFT},   {'W', 26, MOD_SHIFT_LEFT},
    {'E', 8, MOD_SHIFT_LEFT},    {'R', 21, MOD_SHIFT_LEFT},
    {'T', 23, MOD_SHIFT_LEFT},   {'Z', 28, MOD_SHIFT_LEFT},
    {'U', 24, MOD_SHIFT_LEFT},   {'I', 12, MOD_SHIFT_LEFT},
    {'O', 18, MOD_SHIFT_LEFT},   {'P', 19, MOD_SHIFT_LEFT},
    {0xd5, 47, MOD_SHIFT_LEFT}, //Ő
    {154, 48, MOD_SHIFT_LEFT},  //Ú
    {0xdb, 49, MOD_SHIFT_LEFT}, //Ű
    {'\\', 20, MOD_ALT_RIGHT},   {'|', 26, MOD_ALT_RIGHT},
    {132, 8, MOD_ALT_RIGHT},  //Ä
    {172, 24, MOD_ALT_RIGHT}, //€
    {183, 47, MOD_ALT_RIGHT}, //÷
    {151, 48, MOD_ALT_RIGHT}, //×
    {'y', 29, MOD_NO_MODIFIER},  {'x', 27, MOD_NO_MODIFIER},
    {'c', 6, MOD_NO_MODIFIER},   {'v', 25, MOD_NO_MODIFIER},
    {'b', 5, MOD_NO_MODIFIER},   {'n', 17, MOD_NO_MODIFIER},
    {'m', 16, MOD_NO_MODIFIER},  {',', 54, MOD_NO_MODIFIER},
    {'.', 55, MOD_NO_MODIFIER},  {'-', 56, MOD_NO_MODIFIER},
    {'Y', 29, MOD_SHIFT_LEFT},   {'X', 27, MOD_SHIFT_LEFT},
    {'C', 6, MOD_SHIFT_LEFT},    {'V', 25, MOD_SHIFT_LEFT},
    {'B', 5, MOD_SHIFT_LEFT},    {'N', 17, MOD_SHIFT_LEFT},
    {'M', 16, MOD_SHIFT_LEFT},   {'?', 54, MOD_SHIFT_LEFT},
    {':', 55, MOD_SHIFT_LEFT},   {'_', 56, MOD_SHIFT_LEFT},
    {'>', 29, MOD_ALT_RIGHT},    {'#', 27, MOD_ALT_RIGHT},
    {'&', 6, MOD_ALT_RIGHT},     {'@', 25, MOD_ALT_RIGHT},
    {'{', 5, MOD_ALT_RIGHT},     {'}', 17, MOD_ALT_RIGHT},
    {'<', 16, MOD_ALT_RIGHT},    {';', 54, MOD_ALT_RIGHT},
    {'>', 55, MOD_ALT_RIGHT},    {'*', 56, MOD_ALT_RIGHT},
    {173, 100, MOD_NO_MODIFIER}, //í
    {141, 100, MOD_SHIFT_LEFT},  //Í
    {'<', 100, MOD_ALT_RIGHT},   {'\0', 0, MOD_NO_MODIFIER},
};

Keys *keys = &HUN_keys; // keyboard language

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
    for (int i = 0; data[i] != '\0'; i++) {
        for (uint8_t k = 0; k < NumberOfKeys; k++) {
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
