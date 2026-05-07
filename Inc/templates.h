#ifndef TEMPLATES_H
#define TEMPLATES_H

#define NUM_WORDS 2
#define NUM_FEATURES 3

const float word_templates[NUM_WORDS][NUM_FEATURES] = {
    {1.000000, -1.000000, 1.000000},  /* word 0: "on"  */
    {-1.000000, 1.000000, -1.000000}, /* word 1: "off" */
};

const float feature_mean[NUM_FEATURES] = {
    0.064738, 0.256171, 0.050952};

const float feature_std[NUM_FEATURES] = {
    0.020025, 0.050160, 0.016271};

#endif
