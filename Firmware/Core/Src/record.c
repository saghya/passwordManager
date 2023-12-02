#include <stdint.h>
#include <string.h>
#include "record.h"
#include "buttons.h"
#include "flash.h"
#include "chacha.h"
#include "keyboard.h"
#include "screen.h"
#include "pin.h"
#include "encoder.h"
#include "userdata.h"

static inline void    setChar(char *str, uint8_t *selected);
static void           drawChars(char *str, uint8_t selected);
static uint8_t        modifyString(char *title, char str[STR_LEN]);
static inline uint8_t readSites();
static int32_t        findFirstFreeRecordIdx(const Menu *menu);
static uint8_t        saveRecordAt(uint32_t addr, Record *rcrd);
static uint8_t        editRecordLoop(Record *rec);
static inline uint8_t newRecordLoop();
static uint8_t        modifyRecordLoop();
static uint8_t        deleteRecordLoop();

static chacha_context ctx;
static char          *sites[MAX_NUM_OF_RECORDS] = {0};
static Record        *r                         = (Record *)FIRST_RECORD_ADDR;

void Record_Create(Record *rcrd, uint8_t *site, uint8_t *username, uint8_t *password, uint8_t tabnum)
{
    memset(rcrd, 0, sizeof(Record));
    memcpy(rcrd->site, site, strlen((char *)site));
    memcpy(rcrd->username, username, strlen((char *)username));
    memcpy(rcrd->password, password, strlen((char *)password));
    rcrd->tabnum = tabnum;
}

uint8_t Record_Save(const Record *record)
{
    uint32_t addr = 0;
    Record  *r    = (Record *)FIRST_RECORD_ADDR;
    UserData u    = {0};

    for (int i = 0; (uint32_t)&r[i] <= LAST_RECORD_ADDR; i++) {
        if (r[i].xValid) {
            addr = (uint32_t)&r[i];
            break;
        }
    }

    if (!addr || !unlock() || !UserData_Read(USER_DATA_ADDR, &u)) {
        return 0;
    }

    chacha_init(&ctx, (uint8_t *)u.recordKey, (uint8_t *)u.recordNonce);
    chacha_xor(&ctx, (uint8_t *)record->username, sizeof(record->username));
    chacha_xor(&ctx, (uint8_t *)record->password, sizeof(record->password));
    Flash_Write_Page(addr, (uint64_t *)record, sizeof(Record) / 8);
    Screen_SendStatus(&Font_7x10, "Record saved");
    return 1;
}

uint8_t Record_Read(uint32_t addr, Record *record)
{
    UserData u = {0};
    if (addr < FIRST_RECORD_ADDR || addr > LAST_RECORD_ADDR || !UserData_Read(USER_DATA_ADDR, &u))
        return 0;

    Flash_Read_Data(addr, (uint64_t *)record, sizeof(Record) / 8);

    chacha_init(&ctx, (uint8_t *)u.recordKey, (uint8_t *)u.recordNonce);
    chacha_xor(&ctx, (uint8_t *)record->username, sizeof(record->username));
    chacha_xor(&ctx, (uint8_t *)record->password, sizeof(record->password));
    return 1;
}

uint8_t Record_Type(uint32_t addr)
{
    Record r = {0};
    if (!Record_Read(addr, &r))
        return 0;
    typeString((char *)r.username);
    for (int i = 0; i < r.tabnum; i++)
        typeString("\t");
    typeString((char *)r.password);
    typeString("\n");
    memset(&r, 0x00, sizeof(Record));
    return 1;
}

uint8_t Record_Delete(uint32_t addr)
{
    if (!unlock())
        return 1;
    Record r = {.site = {0}, .tabnum = 0, .xValid = 1, .username = {0}, .password = {0}};
    Flash_Write_Page(addr, (uint64_t *)&r, sizeof(Record) / 8);
    Screen_SendStatus(&Font_7x10, "Record deleted");
    return 0;
}

uint8_t Record_DeleteAll()
{
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t                      PAGEError;

    if (!unlock()) {
        return 0;
    }
    HAL_FLASH_Unlock();

    uint32_t StartPage = NUM_OF_PAGES - NUM_OF_RECORD_PAGES;
    uint32_t EndPage   = NUM_OF_PAGES - 1;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page      = StartPage;
    EraseInitStruct.NbPages   = EndPage - StartPage + 1;

    HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
    HAL_FLASH_Lock();

    return 1;
}

void Record_SitesLoop()
{
    if (!readSites()) {
        return;
    }
    Menu menu = Screen_MenuInit(&Font_11x18, "Sites", &Font_7x10, sites);

    while (unlock()) {
        if (btn1()) {
            return;
        }
        if (btn2() || e_sw()) {
            int rIdx = findFirstFreeRecordIdx(&menu);
            if (rIdx < 0 || !Record_Type((uint32_t)&r[rIdx])) {
                return;
            }
        }

        Screen_MenuDraw(&menu);
    }
}

void Record_MenuLoop()
{
    enum { NEW, MODIFY, DELETE };
    char *m[]  = {"New", "Modify", "Delete", NULL};
    Menu  menu = Screen_MenuInit(&Font_11x18, "Records", &Font_7x10, m);
    while (unlock()) {
        if (btn1()) {
            return;
        }
        if (btn2() || e_sw()) {
            switch (menu.selected_string_idx) {
                case NEW:
                    newRecordLoop();
                    break;
                case MODIFY:
                    modifyRecordLoop();
                    break;
                case DELETE:
                    deleteRecordLoop();
                    break;
                default:
                    break;
            }
        }

        Screen_MenuDraw(&menu);
    }
}

static inline void setChar(char *str, uint8_t *selected)
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

static void drawChars(char *str, uint8_t selected)
{
    FontDef charFont = Font_7x10;
    uint8_t x, y;
    for (int i = 0; i < STR_LEN; i++) {
        x = (i % (SSD1306_WIDTH / (charFont.width + 1))) * (charFont.width + 1);
        y = (SSD1306_HEIGHT - charFont.height) / 2 + (i / (SSD1306_WIDTH / (charFont.width + 1))) * (charFont.height + 1);
        ssd1306_SetCursor(x, y);
        ssd1306_WriteChar(str[i], charFont, i == selected ? Black : White);
    }

    ssd1306_UpdateScreen();
}

static uint8_t modifyString(char *title, char str[STR_LEN])
{
    FontDef font         = Font_11x18;
    uint8_t selectedChar = 0;
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(title, font, Black);
    char str2[STR_LEN] = {0};
    strcpy(str2, str);

    while (unlock()) {
        if (btn1()) {
            memset(str2, 0, STR_LEN);
            return 0;
        }
        if (btn2()) {
            strcpy(str, str2);
            return 1;
        }
        setChar(str2, &selectedChar);
        drawChars(str2, selectedChar);
    }
    return 0;
}

static inline uint8_t readSites()
{
    memset(sites, 0, sizeof(sites));
    for (int i = 0, j = 0; (uint32_t)&r[i] <= LAST_RECORD_ADDR; i++) {
        if (r[i].xValid == 0) {
            sites[j++] = (char *)&(r[i].site);
        }
    }
    if (sites[0] == NULL) {
        Screen_SendStatus(&Font_7x10, "No saved record");
        return 0;
    }
    return 1;
}

static int32_t findFirstFreeRecordIdx(const Menu *menu)
{
    for (int i = 0, cnt = -1; (uint32_t)&r[i] <= LAST_RECORD_ADDR; i++) {
        if (r[i].xValid == 0)
            cnt++;
        if (cnt == menu->selected_string_idx) {
            return i;
        }
    }
    return -1;
}

static uint8_t saveRecordAt(uint32_t addr, Record *record)
{
    UserData u = {0};
    if (!addr || !unlock() || !UserData_Read(USER_DATA_ADDR, &u))
        return 1;

    chacha_init(&ctx, (uint8_t *)u.recordKey, (uint8_t *)u.recordNonce);
    chacha_xor(&ctx, (uint8_t *)record->username, sizeof(record->username));
    chacha_xor(&ctx, (uint8_t *)record->password, sizeof(record->password));
    Flash_Write_Page(addr, (uint64_t *)record, sizeof(Record) / 8);
    Screen_SendStatus(&Font_7x10, "Record saved");
    return 0;
}

static uint8_t editRecordLoop(Record *record)
{
    enum { SITE, USERNAME, PASSWORD, SAVE };
    char   *m[]  = {"Site", "Username", "Password", "Save", NULL};
    Menu    menu = Screen_MenuInit(&Font_11x18, "Edit record", &Font_7x10, m);
    uint8_t next = 0;

    while (unlock()) {
        if (btn1()) {
            return 0;
        }
        if (btn2() || e_sw()) {
            switch (menu.selected_string_idx) {
                case SITE:
                    next = modifyString("Site:", (char *)&record->site);
                    break;
                case USERNAME:
                    next = modifyString("Username:", (char *)&record->username);
                    break;
                case PASSWORD:
                    next = modifyString("Password:", (char *)&record->password);
                    break;
                case SAVE:
                    if (record->site[0] && Screen_Question()) {
                        record->tabnum = *record->username && *record->password;
                        return 1;
                    }
                    break;
                default:
                    break;
            }
            menu.selected_row_idx += next;
        }
        Screen_MenuDraw(&menu);
    }
    return 0;
}

static inline uint8_t newRecordLoop()
{
    Record record = {0};
    if (unlock() && editRecordLoop(&record) && Record_Save(&record)) {
        return 1;
    }
    return 0;
}

static uint8_t modifyRecordLoop()
{
    if (!readSites())
        return 1;
    Menu menu = Screen_MenuInit(&Font_11x18, "Modify", &Font_7x10, sites);

    while (unlock()) {
        if (btn1()) {
            return 1;
        }
        if (btn2() || e_sw()) {
            int    rIdx = findFirstFreeRecordIdx(&menu);
            Record rec  = {0};
            if (rIdx < 0 || !Record_Read((uint32_t)&r[rIdx], &rec))
                return 0;
            if (editRecordLoop(&rec)) {
                if (!saveRecordAt((uint32_t)&r[rIdx], &rec))
                    return 0;
            }
        }
        Screen_MenuDraw(&menu);
    }
    return 1;
}

static uint8_t deleteRecordLoop()
{
    if (!readSites()) {
        return 1;
    }
    Menu menu = Screen_MenuInit(&Font_11x18, "Delete", &Font_7x10, sites);

    while (unlock()) {
        if (btn1()) {
            return 1;
        }
        if (btn2() || e_sw()) {
            int rIdx = findFirstFreeRecordIdx(&menu);
            if (rIdx < 0)
                return 0;
            if (Screen_Question()) {
                return Record_Delete((uint32_t)&r[rIdx]);
            }
        }
        Screen_MenuDraw(&menu);
    }
    return 1;
}

