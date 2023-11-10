#include "record.h"
#include "flash.h"
#include <stdint.h>
#include <string.h>
#include "chacha.h"

static chacha_context ctx;
static uint8_t        key[32]   = "key";
static uint8_t        nonce[12] = "nonce";

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
    record  *r    = (record*)FIRST_RECORD_ADDR;
    uint32_t addr = 0;
    for (int i = 0; (uint32_t)&r[i] <= LAST_RECORD_ADDR; i++) {
        if (r[i].xValid) {
            addr = (uint32_t)&r[i];
            break;
        }
    }
    if (!addr) return;
    chacha_init(&ctx, key, nonce);
    chacha_xor(&ctx, (uint8_t *)rcrd->username, sizeof(rcrd->username));
    chacha_xor(&ctx, (uint8_t *)rcrd->password, sizeof(rcrd->password));
    Flash_Write_Page(addr, (uint64_t *)rcrd, sizeof(record) / 8);
}

void read_record(uint32_t addr, record *rcrd)
{
    chacha_init(&ctx, key, nonce);
    Flash_Read_Data(addr, (uint64_t *)rcrd, sizeof(record) / 8);
    chacha_xor(&ctx, (uint8_t *)rcrd->username, sizeof(rcrd->username));
    chacha_xor(&ctx, (uint8_t *)rcrd->password, sizeof(rcrd->password));
}

void delete_record(uint32_t addr)
{
    record r = {.site = {0}, .tabnum = 0, .xValid = 1, .username = {0}, .password = {0}};
    Flash_Write_Page(addr, (uint64_t *)&r, sizeof(record) / 8);
}

