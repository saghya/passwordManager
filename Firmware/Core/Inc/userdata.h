#ifndef __USER_DATA_H
#define __USER_DATA_H

#include "main.h"
#include "keyboard.h"
#include <stdint.h>

#define USER_DATA_ADDR ((uint32_t)(FIRST_RECORD_ADDR - FLASH_PAGE_SIZE)) // directly before records

typedef struct __attribute__((packed, aligned(8))) {
    uint8_t  hashedPW[32];
    uint32_t keyNonce[3];
    uint32_t recordKey[8];
    uint32_t recordNonce[3];
    Keys    *keyboard;
    uint8_t  xValid;
} UserData;

void    UserData_Create();
uint8_t UserData_Save(UserData *u);
uint8_t UserData_Read(uint32_t addr, UserData *u);

#endif // __USER_DATA_H

