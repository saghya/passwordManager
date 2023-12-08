#include "keyboard.h"
#include "pin.h"
#include "userdata.h"

#define USB_DELAY 20

keyboardHID keyboardhid = {0};


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
    Key *keys = ((UserData*)USER_DATA_ADDR)->keyboard;
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

