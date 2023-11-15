#include "record.h"
#include "buttons.h"
#include "flash.h"
#include <stdint.h>
#include <string.h>
#include "chacha.h"
#include "keyboard.h"
#include "screen.h"
#include "pin.h"
#include "encoder.h"
#include "stm32l4xx_hal.h"

static chacha_context ctx;
static uint8_t        key[32]                   = "key";
static uint8_t        nonce[12]                 = "nonce";
char                 *sites[MAX_NUM_OF_RECORDS] = {0};
volatile uint8_t      inSites                   = 0;
static record        *r                         = (record *)FIRST_RECORD_ADDR;

static inline void readSites()
{
    memset(sites, 0, sizeof(sites));
    for (int i = 0, j = 0; (uint32_t)&r[i] <= LAST_RECORD_ADDR; i++) {
        if (r[i].xValid == 0) {
            sites[j++] = (char *)&(r[i].site);
        }
    }
}

void sitesLoop()
{
    readSites();

    //static Page    page;
    //static uint8_t pageInitialized = 0;
    //if (pageInitialized == 0) { // only init once
    //    pageInitialized = 1;
    //    page            = initPage(&Font_11x18, "Sites:", &Font_7x10, sites);
    //}
    Page page            = initPage(&Font_11x18, "Sites:", &Font_7x10, sites);

    while (1) {
        if (btn1()) {
            return;
        }
        if (btn2() || e_sw()) {
            for (int i = 0, cnt = -1; (uint32_t)&r[i] <= LAST_RECORD_ADDR; i++) {
                if (r[i].xValid == 0)
                    cnt++;
                if (cnt == page.selected_string_idx) {
                    typeRecord((uint32_t)&r[i]);
                    break;
                }
            }
        }

        drawPage(&page);
    }
}

void create_record(record *rcrd, uint8_t *site, uint8_t *username, uint8_t *password, uint8_t tabnum)
{
    memset(rcrd, 0, sizeof(record));
    memcpy(rcrd->site, site, strlen((char *)site));
    memcpy(rcrd->username, username, strlen((char *)username));
    memcpy(rcrd->password, password, strlen((char *)password));
    rcrd->tabnum = tabnum;
}

void save_record(const record *rcrd)
{
    uint32_t addr = 0;
    record  *r    = (record *)FIRST_RECORD_ADDR;
    for (int i = 0; (uint32_t)&r[i] <= LAST_RECORD_ADDR; i++) {
        if (r[i].xValid) {
            addr = (uint32_t)&r[i];
            break;
        }
    }
    if (!addr)
        return;
    chacha_init(&ctx, key, nonce);
    chacha_xor(&ctx, (uint8_t *)rcrd->username, sizeof(rcrd->username));
    chacha_xor(&ctx, (uint8_t *)rcrd->password, sizeof(rcrd->password));
    Flash_Write_Page(addr, (uint64_t *)rcrd, sizeof(record) / 8);
    sendStatus(&Font_7x10, "Record saved");
}

void save_record_at(uint32_t addr, record *rcrd)
{
    if (!addr)
        return;

    chacha_init(&ctx, key, nonce);
    chacha_xor(&ctx, (uint8_t *)rcrd->username, sizeof(rcrd->username));
    chacha_xor(&ctx, (uint8_t *)rcrd->password, sizeof(rcrd->password));
    Flash_Write_Page(addr, (uint64_t *)rcrd, sizeof(record) / 8);
    sendStatus(&Font_7x10, "Record saved");
}

void read_record(uint32_t addr, record *rcrd)
{
    chacha_init(&ctx, key, nonce);
    Flash_Read_Data(addr, (uint64_t *)rcrd, sizeof(record) / 8);
    chacha_xor(&ctx, (uint8_t *)rcrd->username, sizeof(rcrd->username));
    chacha_xor(&ctx, (uint8_t *)rcrd->password, sizeof(rcrd->password));
}

void typeRecord(uint32_t addr)
{
    record r = {0};
    read_record(addr, &r);
    typeString((char *)r.username);
    for (int i = 0; i < r.tabnum; i++)
        typeString("\t");
    typeString((char *)r.password);
    typeString("\n");
    memset(&r, 0x00, sizeof(record));
}

void delete_record(uint32_t addr)
{
    record r = {.site = {0}, .tabnum = 0, .xValid = 1, .username = {0}, .password = {0}};
    Flash_Write_Page(addr, (uint64_t *)&r, sizeof(record) / 8);
    sendStatus(&Font_7x10, "Record deleted");
}

uint32_t delete_all_records()
{
    uint32_t                      retval = 0;
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t                      PAGEError;

    HAL_FLASH_Unlock();

    uint32_t StartPage = NUM_OF_PAGES - NUM_OF_RECORD_PAGES;
    uint32_t EndPage   = NUM_OF_PAGES - 1;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page      = StartPage;
    EraseInitStruct.NbPages   = EndPage - StartPage + 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) {
        retval = HAL_FLASH_GetError();
    }
    HAL_FLASH_Lock();

    return retval;
}

void setChar(char *str, uint8_t *selected)
{
    if (e_sw()) {
        *selected = (*selected + 1) % STR_LEN;
    }
    if (str[*selected] == 0x00) {
        str[*selected] = 'a';
    }

    str[*selected] += encoderDelta();
    // handle overflow
    if (str[*selected] > 0x7E) { // '~'
        str[*selected] = 0x20;   // ' '
    } else if (str[*selected] < 0x20) {
        str[*selected] = 0x7E;
    }
}

void drawChars(char *str, uint8_t selected)
{
    FontDef charFont = Font_7x10;
    uint8_t x, y;
    for (int i = 0; i < STR_LEN; i++) {
        x = (i % (SSD1306_WIDTH / (charFont.width + 1))) * (charFont.width + 1);
        y = (SSD1306_HEIGHT - charFont.height) / 2 + (i / (SSD1306_WIDTH / (charFont.width + 1))) * (charFont.height + 1);
        ssd1306_SetCursor(x, y);
        ssd1306_WriteChar(str[i], charFont, i == selected ? Black : White);
        //ssd1306_WriteChar(i == selected ? str[i] : '*', digitFont,
        //                  i == selected ? Black : White);
    }

    ssd1306_UpdateScreen();
}

void modifyString(char *title, char str[STR_LEN])
{
    FontDef font         = Font_11x18;
    uint8_t selectedChar = 0;
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(title, font, Black);
    char str2[STR_LEN] = {0};
    strcpy(str2, str);

    while (1) {
        if (btn1()) {
            memset(str2, 0, STR_LEN);
            return;
        }
        if (btn2()) {
            strcpy(str, str2);
            return;
        }
        setChar(str2, &selectedChar);
        drawChars(str2, selectedChar);
    }
}

uint8_t editRecordLoop(record *rec)
{
    enum { SITE, USERNAME, PASSWORD, SAVE };
    char *m[]  = {"Site", "Username", "Password", "Save", NULL};
    Page  page = initPage(&Font_11x18, "Edit record:", &Font_7x10, m);

    while (1) {
        if (btn1()) {
            return 0;
        }
        if (btn2() || e_sw()) {
            switch (page.selected_string_idx) {
                case SITE:
                    modifyString("Site:", (char *)&rec->site);
                    break;
                case USERNAME:
                    modifyString("Username:", (char *)&rec->username);
                    break;
                case PASSWORD:
                    modifyString("Password:", (char *)&rec->password);
                    break;
                case SAVE:
                    if (rec->site[0] && question()) {
                        return 1;
                    }
                    break;
                default:
                    break;
            }
            page.selected_row_idx++;
        }
        drawPage(&page);
    }
}

void newLoop()
{
    record rec = {0};
    rec.tabnum = 1;
    if (editRecordLoop(&rec))
        save_record(&rec);
}

void modifyLoop()
{
    readSites();
    Page page = initPage(&Font_11x18, "Modify", &Font_7x10, sites);

    while (1) {
        if (btn1()) {
            return;
        }
        if (btn2() || e_sw()) {
            for (int i = 0, cnt = -1; (uint32_t)&r[i] <= LAST_RECORD_ADDR; i++) {
                if (r[i].xValid == 0)
                    cnt++;
                if (cnt == page.selected_string_idx) {
                    record rec = {0};
                    read_record((uint32_t)&r[i], &rec);
                    if (editRecordLoop(&rec)) {
                        save_record_at((uint32_t)&r[i], &rec);
                        return;
                    }
                    break;
                }
            }
        }
        drawPage(&page);
    }
}

void deleteLoop()
{
    readSites();
    Page page = initPage(&Font_11x18, "Delete", &Font_7x10, sites);

    while (1) {
        if (btn1()) {
            return;
        }
        if (btn2() || e_sw()) {
            for (int i = 0, cnt = -1; (uint32_t)&r[i] <= LAST_RECORD_ADDR; i++) {
                if (r[i].xValid == 0)
                    cnt++;
                if (cnt == page.selected_string_idx) {
                    if (question() && getPin()) {
                        delete_record((uint32_t)&r[i]);
                        return;
                    }
                    break;
                }
            }
        }
        drawPage(&page);
    }
}

void recordsLoop()
{
    enum { NEW, MODIFY, DELETE };
    char *m[]  = {"New", "Modify", "Delete", NULL};
    Page  page = initPage(&Font_11x18, "Records:", &Font_7x10, m);
    while (1) {
        if (btn1()) {
            return;
        }
        if (btn2() || e_sw()) {
            switch (page.selected_string_idx) {
                case NEW:
                    newLoop();
                    break;
                case MODIFY:
                    modifyLoop();
                    break;
                case DELETE:
                    deleteLoop();
                    break;
                default:
                    break;
            }
        }

        drawPage(&page);
    }
}

