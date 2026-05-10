#ifndef TEMPLATES_H
#define TEMPLATES_H

#define NUM_WORDS 4
#define NUM_FEATURES 3

const float word_templates[NUM_WORDS][NUM_FEATURES] = {
    {0.320380, 0.047184, 0.239951}, // cairo
    {0.204800, 0.019647, 0.176569}, // Asyut
    {0.444691, 0.032212, 0.313046}, // gharbia
    {0.153787, 0.041031, 0.116604}  // kena
                                    // {0.495752, 0.031269, 0.431286}, // left
                                    // {0.372210, 0.036372, 0.328329}, // right
                                    // {0.515512, 0.022459, 0.457654}, // up
                                    // {0.430202, 0.031766, 0.373393}, // jump
};

const float feature_mean[NUM_FEATURES] = {
    0.071147, 0.247137, 0.055601};

const float feature_std[NUM_FEATURES] = {
    0.016716, 0.045781, 0.014049};

#endif
