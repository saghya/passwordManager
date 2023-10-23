#include "settings.h"
#include "page.h"
#include "encoder.h"
#include "buttons.h"
#include "pin.h"

char *settings[] = {"Keyboard language", "Target device", "Change PIN", NULL};
char *keyboardlanguages[] = {"HU", "EN", NULL};
char *devices[]           = {"Windows", "Linux", "Android", "Iphone", NULL};

void setKeyboardLanguage()
{
    static Page    page;
    static uint8_t pageInitialized = 0;
    if (pageInitialized == 0) { // only init once
        pageInitialized = 1;
        page            = initPage(&Font_7x10, "Keyboard language:", &Font_7x10,
                                   keyboardlanguages);
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
    static Page    page;
    static uint8_t pageInitialized = 0;
    if (pageInitialized == 0) { // only init once
        pageInitialized = 1;
        page = initPage(&Font_7x10, "Target device:", &Font_7x10, devices);
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
    static Page    page;
    static uint8_t pageInitialized = 0;
    if (pageInitialized == 0) { // only init once
        pageInitialized = 1;
        page = initPage(&Font_11x18, "Settings:", &Font_7x10, settings);
    }
    while (1) {
        if (btn1()) {
            return;
        }
        if (btn2() || e_sw()) {
            switch (page.selected_string_idx) {
                case SET_KEYBOARD_LANGUAGE:
                    setKeyboardLanguage();
                    break;
                case SET_DEVICE:
                    setDevice();
                    break;
                case SET_PIN:
                    changePin();
                    break;
            }
        }

        drawPage(&page);
    }
}
