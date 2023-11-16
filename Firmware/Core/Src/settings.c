#include "settings.h"
#include "keyboard.h"
#include "screen.h"
#include "encoder.h"
#include "buttons.h"
#include "pin.h"
#include "record.h"
#include "userdata.h"
#include <stdint.h>

uint8_t setKeyboardLanguage()
{
    UserData u = {0};
    if (!readUserData(USER_DATA_ADDR, &u))
        return 0;
    char   *keyboardlanguages[] = {"HU", "US", NULL};
    Page    page                = initPage(&Font_7x10, "Keyboard language:", &Font_7x10, keyboardlanguages);
    uint8_t idx                 = 0;
    if (u.keyboard == US_keys) {
        idx = page.selected_row_idx = 1;
    }

    while (1) {
        if (btn1()) { // cancel
            return 1;
        } else if (btn2() || e_sw()) {
            if (page.selected_row_idx != idx) { // only save changes if changed
                u.keyboard = page.selected_row_idx ? US_keys : HU_keys;
                if (!unlock() || !saveUserData(&u))
                    return 0;
            }
            sendStatus(&Font_7x10, "Keyboard set");
            return 1;
        }

        drawPage(&page);
    }
}

void settingsLoop()
{
    enum { KEYBOARD_LANGUAGE, CHANGE_PIN, FACTORY_RESET };
    char *settings[] = {"Keyboard language", "Change PIN", "Factory reset", NULL};

    Page page = initPage(&Font_11x18, "Settings", &Font_7x10, settings);
    while (1) {
        if (btn1()) {
            return;
        }
        if (btn2() || e_sw()) {
            switch (page.selected_string_idx) {
                case KEYBOARD_LANGUAGE:
                    if (!setKeyboardLanguage()) {
                        return;
                    }
                    break;
                case CHANGE_PIN:
                    if (!changePin()) {
                        return;
                    }
                    break;
                case FACTORY_RESET:
                    if (unlock() && question()) {
                        sendStatus(&Font_7x10, "Reseted");
                        deleteAllRecords();
                        createUser();
                    }
                    break;
            }
        }

        drawPage(&page);
    }
}

