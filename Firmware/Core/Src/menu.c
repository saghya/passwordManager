#include "menu.h"
#include "page.h"
#include "encoder.h"
#include "buttons.h"
#include "pin.h"
#include "sites.h"
#include "settings.h"

char *menus[] = { "Sites", "Settings", NULL };

void menuLoop() { // newer returns
	Page page = initPage(&Font_11x18, "PW Manager", &Font_7x10, menus);
	while (1) {
		if (btn1()) {
			//lock();
		}
		if (btn2() || e_sw()) {
			switch (page.selected_string_idx) {
			case M_SITES:
				if (unlock()) {
					sitesLoop();
				}
				break;
			case M_SETTINGS:
				settingsLoop();
				break;
			}
		}

		drawPage(&page);
	}
}
