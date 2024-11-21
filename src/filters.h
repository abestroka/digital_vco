#ifndef FILTERS_H
#define FILTERS_H


#include <stdint.h>
#include "pico/audio_i2s.h"
#include "envelope.h"
#include "oscillators.h"


void init_lowpass(float cutoff_freq, float *ampin0, float *ampin1, float *ampin2, float *ampout1, float *ampout2);
void init_highpass(float cutoff_freq, float *ampin0, float *ampin1, float *ampin2, float *ampout1, float *ampout2);
void init_bandpass(float cutoff_freq, float *ampin0, float *ampin1, float *ampin2, float *ampout1, float *ampout2);

#endif // FILTERS_H