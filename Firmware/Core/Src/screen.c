#include <string.h>
#include "screen.h"
#include "encoder.h"
#include "buttons.h"

uint8_t rows_indent = 5;

Menu Screen_MenuInit(FontDef *title_font, char *title_str, FontDef *rows_font, char *rows_str[])
{
    Menu menu = {0};

    // set variables
    if (title_font != NULL && title_str != NULL) {
        menu.title_font = *title_font;
    }
    if (rows_font != NULL && rows_str != NULL) {
        menu.rows_font = *rows_font;
    }
    menu.title = title_str;
    menu.rows  = rows_str;

    // calculate number of strings
    for (menu.strs_len = 0; menu.rows[menu.strs_len] != NULL; (menu.strs_len)++)
        ;

    // calculate how many rows will fit the screen
    menu.num_rows = (SSD1306_HEIGHT - 1 - (menu.title_font.height + 1)) / (menu.rows_font.height + 1);
    if (menu.num_rows > menu.strs_len) {
        menu.num_rows = menu.strs_len;
    }

    // start drawing rows from the bottom
    menu.rows_offset = SSD1306_HEIGHT - menu.num_rows * (menu.rows_font.height + 1) - menu.title_font.height;
    // center rows
    menu.rows_offset /= 2;

    return menu;
}

void Screen_MenuDraw(Menu *menu)
{
    menu->selected_row_idx += encoderDelta();

    // handle overflow
    if (menu->selected_row_idx < 0) {
        if (menu->string_offset == 0) {
            menu->string_offset    = menu->strs_len - menu->num_rows;
            menu->selected_row_idx = menu->num_rows - 1;
        } else {
            (menu->string_offset)--;
            menu->selected_row_idx = 0;
        }
    } else if (menu->selected_row_idx > menu->num_rows - 1) {
        if (menu->string_offset == menu->strs_len - menu->num_rows) {
            menu->string_offset    = 0;
            menu->selected_row_idx = 0;
        } else {
            (menu->string_offset)++;
            menu->selected_row_idx = menu->num_rows - 1;
        }
    }
    menu->selected_string_idx = menu->string_offset + menu->selected_row_idx;

    ssd1306_Fill(Black); // clear screen

    // draw title
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(menu->title, menu->title_font, Black);

    // draw rows
    if (menu->rows != NULL) {
        for (int i = 0; i + menu->string_offset < menu->strs_len && i < menu->num_rows; i++) {
            ssd1306_SetCursor(rows_indent, (menu->rows_font.height + 1) * i + menu->title_font.height + menu->rows_offset);
            ssd1306_WriteString(menu->rows[i + menu->string_offset], menu->rows_font,
                                i == menu->selected_row_idx ? Black : White);
        }
    }

    ssd1306_UpdateScreen();
}

void Screen_SendStatus(FontDef *font, char *str)
{
    ssd1306_Fill(Black);
    ssd1306_SetCursor((SSD1306_WIDTH - font->width * strlen(str)) / 2, (SSD1306_HEIGHT - font->height) / 2); // center
    ssd1306_WriteString(str, *font, Black);
    ssd1306_UpdateScreen();
    HAL_Delay(1000);
}

uint8_t Screen_Question()
{
    enum { NO, YES };
    char *options[] = {"no", "yes", NULL};
    Menu  menu      = Screen_MenuInit(&Font_7x10, "Are you sure?", &Font_7x10, options);
    while (1) {
        if (btn1()) {
            return 0;
        }
        if (btn2() || e_sw()) {
            switch (menu.selected_string_idx) {
                case YES:
                    return 1;
                case NO:
                    return 0;
            }
        }
        Screen_MenuDraw(&menu);
    }
}

