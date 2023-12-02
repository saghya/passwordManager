#include "menu.h"
#include "screen.h"
#include "buttons.h"
#include "pin.h"
#include "settings.h"
#include "record.h"

enum { SITES, RECORDS, SETTINGS };
char *sub_menus[] = {"Sites", "Records", "Settings", NULL};

void MainMenu_Loop()
{
    Menu menu = Screen_MenuInit(&Font_11x18, "PW Manager", &Font_7x10, sub_menus);
    while (unlock()) {
        if (btn1()) {
            lock();
            return;
        }
        if (btn2() || e_sw()) {
            switch (menu.selected_string_idx) {
                case SITES:
                    Record_SitesLoop();
                    break;
                case RECORDS:
                    Record_MenuLoop();
                    break;
                case SETTINGS:
                    Settings_Loop();
                    break;
            }
        }

        Screen_MenuDraw(&menu);
    }
}

