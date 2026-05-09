#include "features.h"
#include <math.h>

#define FRAME_SIZE 200

Features extract_frame_features(const uint8_t *frame)
{
    int32_t sum_sq = 0;
    uint32_t sum_abs = 0;
    uint8_t crossings = 0;
    int8_t prev_sign = 0;

    for (int i = 0; i < FRAME_SIZE; i++)
    {
        int8_t s = (int8_t)((int16_t)frame[i] - 122);

        sum_sq += (int32_t)s * s;

        uint8_t abs_s = (s >= 0) ? (uint8_t)s : (uint8_t)(-s);
        sum_abs += abs_s;

        int8_t curr_sign = (s >= 0) ? 1 : -1;
        if (i > 0 && curr_sign != prev_sign)
            crossings++;
        prev_sign = curr_sign;
    }

    /* fill the struct and return it */
    Features result;
    result.rms = sqrtf((float)sum_sq / FRAME_SIZE) / 128.0f;
    result.zcr = (float)crossings / (2.0f * FRAME_SIZE);
    result.envelope = ((float)sum_abs / FRAME_SIZE) / 128.0f;

    return result;
}