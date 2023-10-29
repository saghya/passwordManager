#ifndef __RECORD_H
#define __RECORD_H

#include "flash.h"

#define NUM_OF_RECORD_PAGES (32)
#define FIRST_RECORD_ADDR                                                      \
    (FIRST_PAGE_ADDR + (NUM_OF_PAGES - NUM_OF_RECORD_PAGES) * FLASH_PAGE_SIZE)

typedef struct __attribute__((packed, aligned(8))) {
    uint8_t site[32];
    uint8_t username[32];
    uint8_t password[32];
    uint8_t tabnum;
} record;

void create_record(record *rcrd, uint8_t *site, uint8_t *username,
                  uint8_t *password, uint8_t tabnum);
void save_record(record *rcrd);
void read_record(record *rcrd);

#endif // __RECORD_H

