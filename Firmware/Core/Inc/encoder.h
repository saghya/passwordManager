#ifndef __ENCODER_H
#define __ENCODER_H

#include "main.h"
#define ENC_CNT ((int16_t)TIM1->CNT)

int8_t encoderDelta();

#endif // __ENCODER_H

