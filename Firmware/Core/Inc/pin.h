#ifndef __PIN_H
#define __PIN_H

#include "main.h"
#include "record.h"

#define DIGITS         6
#define AUTOLOCK_TIMER TIM16->CNT
#define USER_DATA_ADDR ((uint32_t)(FIRST_RECORD_ADDR - FLASH_PAGE_SIZE)) // directly before records

typedef struct __attribute__((packed, aligned(8))) {
    uint8_t  hashedPW[32];
    uint32_t keyNonce[3];
    uint32_t recordKey[8];
    uint32_t recordNonce[3];
    uint8_t  xValid;
} UserData;

extern int8_t pin[DIGITS];

void    lock();
uint8_t unlock();
uint8_t getPin();
uint8_t changePin();
void    createUser();
void    readUserData(uint32_t addr, UserData *u);
uint8_t checkPin();

#endif // __PIN_H
