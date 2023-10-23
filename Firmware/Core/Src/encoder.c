#include <stdlib.h>
#include "encoder.h"

int16_t enc      = 0;
int16_t prev_enc = 0;

int8_t encoderDelta()
{
    int8_t retval;
    enc = ENC_CNT;
    if (abs(enc - prev_enc) > 2) {
        retval   = enc - prev_enc < 0 ? 1 : -1;
        prev_enc = enc;
    } else {
        retval = 0;
    }
    return retval;
}
