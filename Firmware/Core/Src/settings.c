#include <stdint.h>
#include "settings.h"
#include "keyboard.h"
#include "keyboard_languages.h"
#include "screen.h"
#include "buttons.h"
#include "pin.h"
#include "record.h"
#include "userdata.h"

static uint8_t setKeyboardLanguage()
{
    UserData u = {0};
    if (!UserData_Read(USER_DATA_ADDR, &u))
        return 0;
    char   *keyboardlanguages[] = {"HU", "US", NULL};
    Menu    menu                = Screen_MenuInit(&Font_7x10, "Keyboard language:", &Font_7x10, keyboardlanguages);
    uint8_t idx                 = 0;
    if (u.keyboard == US_keys) {
        idx = menu.selected_row_idx = 1;
    }

    while (unlock()) {
        if (btn1()) { // cancel
            return 1;
        } else if (btn2() || e_sw()) {
            if (menu.selected_row_idx != idx) { // only save changes if changed
                u.keyboard = menu.selected_row_idx ? US_keys : HU_keys;
                if (!unlock() || !UserData_Save(&u))
                    return 0;
            }
            Screen_SendStatus(&Font_7x10, "Keyboard set");
            return 1;
        }

        Screen_MenuDraw(&menu);
    }
    return 1;
}

static inline void factoryReset()
{
    if (unlock() && Screen_Question()) {
        Screen_SendStatus(&Font_7x10, "Reseted");
        Record_DeleteAll();
        UserData_Create();
    }
}

void Settings_Loop()
{
    enum { KEYBOARD_LANGUAGE, CHANGE_PIN, FACTORY_RESET };
    char *settings[] = {"Keyboard language", "Change PIN", "Factory reset", NULL};

    Menu menu = Screen_MenuInit(&Font_11x18, "Settings", &Font_7x10, settings);
    while (unlock()) {
        if (btn1()) {
            return;
        }
        if (btn2() || e_sw()) {
            switch (menu.selected_string_idx) {
                case KEYBOARD_LANGUAGE:
                    if (!setKeyboardLanguage()) {
                        return;
                    }
                    break;
                case CHANGE_PIN:
                    if (!PIN_Change()) {
                        return;
                    }
                    break;
                case FACTORY_RESET:
                    factoryReset();
                    break;
            }
        }

        Screen_MenuDraw(&menu);
    }
}

