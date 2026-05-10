#ifndef TEMPLATES_H
#define TEMPLATES_H

#define NUM_WORDS 8
#define NUM_FEATURES 3

const float word_templates[NUM_WORDS][NUM_FEATURES] = {
    {0.455485, 0.030762, 0.390127}, // open
    {0.287970, 0.045763, 0.252762}, // off
    {0.322083, 0.038619, 0.282691}, // start
    {0.268314, 0.057984, 0.229396}, // stop
    {0.495752, 0.031269, 0.431286}, // left
    {0.372210, 0.036372, 0.328329}, // right
    {0.515512, 0.022459, 0.457654}, // up
    {0.430202, 0.031766, 0.373393}, // jump
};

const float feature_mean[NUM_FEATURES] = {
    0.071147, 0.247137, 0.055601};

const float feature_std[NUM_FEATURES] = {
    0.016716, 0.045781, 0.014049};

#endif
