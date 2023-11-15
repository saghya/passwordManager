#ifndef __PIN_H
#define __PIN_H

#include "main.h"
#include "record.h"

#define DIGITS         6
#define AUTOLOCK_TIMER TIM16->CNT
#define USER_DATA_ADDR ((uint32_t)(FIRST_RECORD_ADDR - FLASH_PAGE_SIZE) // directly before records

typedef struct __attribute__((packed, aligned(8))) {
    uint8_t  hashedPW[32];
    uint32_t keyNonce[3];
    uint32_t recordKey[8];
    uint32_t recordNonce[3];
} UserData;

extern uint8_t unlocked;

void    lock();
uint8_t getPin();
void    changePin();
void    createUser();

#endif // __PIN_H
