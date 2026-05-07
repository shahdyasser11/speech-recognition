#ifndef FEATURES_H
#define FEATURES_H

#include <stdint.h>

/* struct holds the 3 features*/
typedef struct
{
    float rms;
    float zcr;
    float envelope;
} Features;

Features extract_frame_features(const uint8_t *frame);

#endif