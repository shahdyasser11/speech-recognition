#ifndef TEMPLATES_H
#define TEMPLATES_H

#define NUM_WORDS 2
#define NUM_FEATURES 3

const float word_templates[NUM_WORDS][NUM_FEATURES] = {
    {1.000000, -1.000000, 1.000000},  /* word 0: "on"  */
    {-1.000000, 1.000000, -1.000000}, /* word 1: "off" */

    //  ON – OFF – START – STOP – LEFT – RIGHT – UP – DOWN
    //   0 -  1  -  2  -   3   -   4   -   5   -  6 -   7
};

const float feature_mean[NUM_FEATURES] = {
    0.064738, 0.256171, 0.050952};

const float feature_std[NUM_FEATURES] = {
    0.020025, 0.050160, 0.016271};

#endif
