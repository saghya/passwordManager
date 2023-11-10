#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "keyboard.h"
#include "sites.h"
#include "page.h"
#include "encoder.h"
#include "pin.h"
#include "buttons.h"
#include "record.h"

char            *sites[MAX_NUM_OF_RECORDS] = {0};
volatile uint8_t inSites    = 0;

void sitesLoop()
{
    record *r = (record*)FIRST_RECORD_ADDR;
    for (int i = 0, j = 0; (uint32_t)&r[i] <= LAST_RECORD_ADDR; i++) {
        if (r[i].xValid == 0) {
            sites[j++] = (char *)&(r[i].site);
        }
    }
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
                for (int i = 0, cnt = -1; (uint32_t)&r[i] <= LAST_RECORD_ADDR; i++) {
                    if (r[i].xValid == 0)
                        cnt++;
                    if (cnt == page.selected_string_idx) {
                        record r1;
                        read_record((uint32_t)&r[i], &r1);
                        typeString((char *)r1.username);
                        for (int i = 0; i < r1.tabnum; i++)
                            typeString("\t");
                        typeString((char *)r1.password);
                        typeString("\n");
                        break;
                    }
                }
            } else {
                unlock();
            }
        }

        drawPage(&page);
    }
}
