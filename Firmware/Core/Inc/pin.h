#ifndef __PIN_H
#define __PIN_H

#include "main.h"
#include "record.h"
#include "keyboard.h"

#define DIGITS         6
#define AUTOLOCK_TIMER TIM16->CNT

extern int8_t pin[DIGITS];

uint8_t PIN_Get();
uint8_t PIN_Set();
uint8_t PIN_Check();
uint8_t PIN_Change();

uint8_t unlock();
void    lock();

#endif // __PIN_H

