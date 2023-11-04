#include "record.h"
#include "flash.h"
#include <stdint.h>
#include <string.h>
#include "chacha.h"

static chacha_context ctx;
static uint8_t key[32] = "key";
static uint8_t nonce[12] = "nonce";

void create_record(record *rcrd, uint8_t *site, uint8_t *username,
                  uint8_t *password, uint8_t tabnum)
{
    memset(rcrd, 0, sizeof(record));
    memcpy(rcrd->site, site, strlen((char *)site));
    memcpy(rcrd->username, username, strlen((char *)username));
    memcpy(rcrd->password, password, strlen((char *)password));
    rcrd->tabnum = tabnum;
}

void save_record(uint32_t addr, record *rcrd)
{
    chacha_init(&ctx, key, nonce);
    chacha_xor(&ctx, (uint8_t*)rcrd, sizeof(record));
    Flash_Write_Page(addr, (uint64_t *)rcrd, sizeof(record) / 8);
}

void read_record(uint32_t addr, record *rcrd)
{
    chacha_init(&ctx, key, nonce);
    Flash_Read_Data(addr, (uint64_t *)rcrd, sizeof(record) / 8);
    chacha_xor(&ctx, (uint8_t*)rcrd, sizeof(record));
}
