#ifndef __PIN_H
#define __PIN_H

#include "main.h"
#include "record.h"
#include "keyboard.h"

#define DIGITS         6
#define AUTOLOCK_TIMER TIM16->CNT

extern int8_t pin[DIGITS];

void    lock();
uint8_t unlock();
uint8_t getPin();
uint8_t changePin();
void    createUser();
uint8_t checkPin();
uint8_t setPin();

#endif // __PIN_H

