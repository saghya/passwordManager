#ifndef __PIN_H
#define __PIN_H

#define DIGITS 6
#define AUTOLOCK_TIMER TIM16->CNT

#include "main.h"

extern uint8_t unlocked;

void lock();
uint8_t unlock();
void changePin();

#endif // __PIN_H
