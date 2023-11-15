#include "settings.h"
#include "screen.h"
#include "encoder.h"
#include "buttons.h"
#include "pin.h"
#include "record.h"

void setKeyboardLanguage()
{
    char          *keyboardlanguages[] = {"HU", "EN", NULL};
    static Page    page;
    static uint8_t pageInitialized = 0;
    if (pageInitialized == 0) { // only init once
        pageInitialized = 1;
        page            = initPage(&Font_7x10, "Keyboard language:", &Font_7x10, keyboardlanguages);
    }
    Page page_copy = page;

    while (1) {
        if (btn1()) { // cancel
            return;
        } else if (btn2() || e_sw()) { // save changes
            page = page_copy;
            return;
        }

        drawPage(&page_copy);
    }
}

void setDevice()
{
    char          *devices[] = {"Windows", "Linux", "Android", "Iphone", NULL};
    static Page    page;
    static uint8_t pageInitialized = 0;
    if (pageInitialized == 0) { // only init once
        pageInitialized = 1;
        page            = initPage(&Font_7x10, "Target device:", &Font_7x10, devices);
    }
    Page page_copy = page;

    while (1) {
        if (btn1()) { // cancel
            return;
        } else if (btn2() || e_sw()) { // save changes
            page = page_copy;
            return;
        }

        drawPage(&page_copy);
    }
}

void settingsLoop()
{
    enum { KEYBOARD_LANGUAGE, TARGET_DEVICE, CHANGE_PIN, FACTORY_RESET };
    char *settings[] = {"Keyboard language", "Target device", "Change PIN", "Factory reset", NULL};

    Page page = initPage(&Font_11x18, "Settings:", &Font_7x10, settings);
    while (1) {
        if (btn1()) {
            return;
        }
        if (btn2() || e_sw()) {
            switch (page.selected_string_idx) {
                case KEYBOARD_LANGUAGE:
                    setKeyboardLanguage();
                    break;
                case TARGET_DEVICE:
                    setDevice();
                    break;
                case CHANGE_PIN:
                    changePin();
                    break;
                case FACTORY_RESET:
                    if (question() && getPin()) {
                        delete_all_records();
                        createUser();
                    }
                    break;
            }
        }

        drawPage(&page);
    }
}

