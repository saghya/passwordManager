#include "menu.h"
#include "screen.h"
#include "encoder.h"
#include "buttons.h"
#include "pin.h"
#include "settings.h"
#include "record.h"

enum { SITES, RECORDS, SETTINGS };
char *menus[] = {"Sites", "Records", "Settings", NULL};

void MainMenu_Loop()
{
    Page page = Screen_PageInit(&Font_11x18, "PW Manager", &Font_7x10, menus);
    while (unlock()) {
        if (btn1()) {
            lock();
            return;
        }
        if (btn2() || e_sw()) {
            switch (page.selected_string_idx) {
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

        Screen_PageDraw(&page);
    }
}

