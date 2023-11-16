#include "menu.h"
#include "screen.h"
#include "encoder.h"
#include "buttons.h"
#include "pin.h"
#include "settings.h"
#include "record.h"

enum { SITES, RECORDS, SETTINGS };
char *menus[] = {"Sites", "Records", "Settings", NULL};

void menuLoop()
{
    Page page = initPage(&Font_11x18, "PW Manager", &Font_7x10, menus);
    while (unlock()) {
        if (btn1()) {
            lock();
            return;
        }
        if (btn2() || e_sw()) {
            switch (page.selected_string_idx) {
                case SITES:
                    sitesLoop();
                    break;
                case RECORDS:
                    recordsLoop();
                    break;
                case SETTINGS:
                    settingsLoop();
                    break;
            }
        }

        drawPage(&page);
    }
}

