#include <string.h>
#include "page.h"
#include "encoder.h"

uint8_t rows_indent = 5;

Page initPage(FontDef *title_font, char *title_str, FontDef *rows_font, char *rows_str[])
{
    Page page = {0};

    // set variables
    if (title_font != NULL && title_str != NULL) {
        page.title_font = *title_font;
    }
    if (rows_font != NULL && rows_str != NULL) {
        page.rows_font = *rows_font;
    }
    page.title = title_str;
    page.rows  = rows_str;

    // calculate number of strings
    for (page.strs_len = 0; page.rows[page.strs_len] != NULL; (page.strs_len)++);

    // calculate how many rows will fit the screen
    page.num_rows = (SSD1306_HEIGHT - 1 - (page.title_font.height + 1)) / (page.rows_font.height + 1);
    if (page.num_rows > page.strs_len) {
        page.num_rows = page.strs_len;
    }

    // start drawing rows from the bottom
    page.rows_offset = SSD1306_HEIGHT - page.num_rows * (page.rows_font.height + 1) - page.title_font.height;
    // center rows
    page.rows_offset /= 2;

    return page;
}

void drawPage(Page *page)
{
    page->selected_row_idx += encoderDelta();

    // handle overflow
    if (page->selected_row_idx < 0) {
        if (page->string_offset == 0) {
            page->string_offset    = page->strs_len - page->num_rows;
            page->selected_row_idx = page->num_rows - 1;
        } else {
            (page->string_offset)--;
            page->selected_row_idx = 0;
        }
    } else if (page->selected_row_idx > page->num_rows - 1) {
        if (page->string_offset == page->strs_len - page->num_rows) {
            page->string_offset    = 0;
            page->selected_row_idx = 0;
        } else {
            (page->string_offset)++;
            page->selected_row_idx = page->num_rows - 1;
        }
    }
    page->selected_string_idx = page->string_offset + page->selected_row_idx;

    ssd1306_Fill(Black); // clear screen

    // draw title
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(page->title, page->title_font, Black);

    // draw rows
    for (int i = 0; i + page->string_offset < page->strs_len && i < page->num_rows; i++) {
        ssd1306_SetCursor(rows_indent, (page->rows_font.height + 1) * i + page->title_font.height + page->rows_offset);
        ssd1306_WriteString(page->rows[i + page->string_offset], page->rows_font, i == page->selected_row_idx ? Black : White);
    }

    ssd1306_UpdateScreen();
}
