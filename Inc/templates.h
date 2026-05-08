#ifndef TEMPLATES_H
#define TEMPLATES_H

#define NUM_WORDS 8
#define NUM_FEATURES 3

const float word_templates[NUM_WORDS][NUM_FEATURES] = {
    {0.814560, -0.897284, 0.827227},
    {-0.858860, 0.959785, -0.851174},
    {-0.196290, 0.914206, -0.243385},
    {-1.240249, 1.104639, -1.226368},
    {0.104041, 0.449637, 0.116269},
    {0.594355, -0.257495, 0.583655},
    {-1.083889, -0.310657, -1.079386},
    {1.866334, -1.962832, 1.873163},
};

const float feature_mean[NUM_FEATURES] = {
    0.071147, 0.247137, 0.055601
};

const float feature_std[NUM_FEATURES] = {
    0.016716, 0.045781, 0.014049
};

#endif
