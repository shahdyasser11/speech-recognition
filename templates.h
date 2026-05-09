#ifndef TEMPLATES_H
#define TEMPLATES_H

#define NUM_WORDS 8
#define NUM_FEATURES 3

const float word_templates[NUM_WORDS][NUM_FEATURES] = {
    {0.725536, -0.897284, 0.722320},
    {-0.597216, 0.959785, -0.614541},
    {-0.841062, 0.914206, -0.838993},
    {-1.144866, 1.104639, -1.142677},
    {0.081526, 0.449637, 0.118538},
    {0.840486, -0.257495, 0.810625},
    {-0.937211, -0.310657, -0.937169},
    {1.872805, -1.962832, 1.881894},
};

const float feature_mean[NUM_FEATURES] = {
    0.053774, 0.247137, 0.041639
};

const float feature_std[NUM_FEATURES] = {
    0.014124, 0.045781, 0.011782
};

#endif
