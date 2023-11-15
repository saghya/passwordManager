#ifndef __RECORD_H
#define __RECORD_H

#include "flash.h"

#define NUM_OF_RECORD_PAGES ((uint32_t)31)
#define FIRST_RECORD_ADDR   (FIRST_PAGE_ADDR + (NUM_OF_PAGES - NUM_OF_RECORD_PAGES) * FLASH_PAGE_SIZE)
#define LAST_RECORD_ADDR    (FIRST_RECORD_ADDR + NUM_OF_RECORD_PAGES * FLASH_PAGE_SIZE - sizeof(record))
#define MAX_NUM_OF_RECORDS  ((LAST_RECORD_ADDR - FIRST_RECORD_ADDR + sizeof(record)) / sizeof(record))
#define STR_LEN             32

typedef struct __attribute__((packed, aligned(8))) {
    uint8_t site[STR_LEN];
    uint8_t tabnum : 7;
    uint8_t xValid : 1;
    uint8_t username[STR_LEN];
    uint8_t password[STR_LEN];
} record;

extern volatile uint8_t inSites;

void     sitesLoop();
void     create_record(record *rcrd, uint8_t *site, uint8_t *username, uint8_t *password, uint8_t tabnum);
void     save_record(const record *rcrd);
void     read_record(uint32_t addr, record *rcrd);
void     typeRecord(uint32_t addr);
void     delete_record(uint32_t addr);
uint32_t delete_all_records();

void recordsLoop();

#endif // __RECORD_H

