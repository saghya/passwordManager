#ifndef __RECORD_H
#define __RECORD_H

#include "flash.h"
#include <stdint.h>

#define NUM_OF_RECORD_PAGES ((uint32_t)31)
#define FIRST_RECORD_ADDR   (FIRST_PAGE_ADDR + (NUM_OF_PAGES - NUM_OF_RECORD_PAGES) * FLASH_PAGE_SIZE)
#define LAST_RECORD_ADDR    (FIRST_RECORD_ADDR + NUM_OF_RECORD_PAGES * FLASH_PAGE_SIZE - sizeof(Record))
#define MAX_NUM_OF_RECORDS  ((LAST_RECORD_ADDR - FIRST_RECORD_ADDR + sizeof(Record)) / sizeof(Record))
#define STR_LEN             32

typedef struct __attribute__((packed, aligned(8))) {
    uint8_t site[STR_LEN];
    uint8_t tabnum;
    uint8_t xValid;
    uint8_t username[STR_LEN];
    uint8_t password[STR_LEN];
} Record;

void    Record_Create(Record *rcrd, uint8_t *site, uint8_t *username, uint8_t *password, uint8_t tabnum);
uint8_t Record_Save(const Record *rcrd);
uint8_t Record_Read(uint32_t addr, Record *rcrd);
uint8_t Record_Type(uint32_t addr);
uint8_t Record_Delete(uint32_t addr);
uint8_t Record_DeleteAll();
void    Record_SitesLoop();
void    Record_MenuLoop();

#endif // __RECORD_H

