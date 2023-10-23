#include <stdlib.h>
#include "sites.h"
#include "page.h"
#include "encoder.h"
#include "passwords.h"
#include "pin.h"
#include "buttons.h"

char *sites[] = {"facebook", "neptun", "bme",    "google", "moodle",  "spotify",
                 "teams",    "steam",  "github", "otp",    "netflix", NULL};

volatile uint8_t inSites = 0;

void sitesLoop()
{
    inSites = 1;
    static Page    page;
    static uint8_t pageInitialized = 0;
    if (pageInitialized == 0) { // only init once
        pageInitialized = 1;
        page            = initPage(&Font_11x18, "Sites:", &Font_7x10, sites);
    }

    while (inSites) {
        if (btn1()) {
            lock();
            inSites = 0;
            return;
        }
        if (btn2() || e_sw()) {
            if (unlocked) {
                sendPassword(sites[page.selected_string_idx]);
            } else {
                unlock();
            }
        }

        drawPage(&page);
    }
}
