#include <stdlib.h>
#include "encoder.h"

static int16_t prev_enc = 0;

int8_t encoderDelta()
{
    int8_t retval = 0;
    if (abs(ENC_CNT - prev_enc) > 3) {
        retval   = (ENC_CNT - prev_enc < 0) ? -1 : +1;
        prev_enc = ENC_CNT;
    }
    return retval;
}
