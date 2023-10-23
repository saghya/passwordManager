#ifndef __DRAW_H
#define __DRAW_H

#include "ssd1306.h"
#include "ssd1306_fonts.h"

// set by page functions
typedef struct {
    uint8_t string_offset;    // index of the first string that is being drawn
    int16_t selected_row_idx; // highlighted row
    uint8_t selected_string_idx; // selected_row + string_offset
    uint8_t num_rows;            // number of strings that fits the screen
    uint8_t strs_len;            // number of strings in the string array
    uint8_t rows_offset; // adjusts the spacing between the title and rows
    FontDef title_font;
    FontDef rows_font;
    char   *title;
    char  **rows;
} Page;

Page initPage(FontDef *title_font, char *title_str, FontDef *rows_font,
              char *rows_str[]);
void drawPage(Page *p);

#endif // __DRAW_H
