#include "filters.h"
#include "oscillators.h"
#include <math.h>
#include "i2s_audio.h"
#include "envelope.h"

void init_lowpass(float cutoff, float *ampin0, float *ampin1, float *ampin2, float *ampout1, float *ampout2) {
    float sqr2 = 1.414213562;
    float c = 1 / tanf((M_PI / SAMPLE_RATE) * cutoff);
    float c2 = c * c;
    float csqr2 = sqr2 * c;
    float d = (c2 + csqr2 + 1);
    
    *ampin0 = 1 / d;
    *ampin1 = *ampin0 + *ampin0;
    *ampin2 = *ampin0;
    *ampout1 = (2 * (1 - c2)) / d;
    *ampout2 = (c2 - csqr2 + 1) / d;
}

void init_highpass(float cutoff, float *ampin0, float *ampin1, float *ampin2, float *ampout1, float *ampout2) {
    float sqr2 = 1.414213562;
    float c = tanf((M_PI / SAMPLE_RATE) * cutoff);
    float c2 = c * c;
    float csqr2 = sqr2 * c;
    float d = (c2 + csqr2 + 1);
    
    *ampin0 = 1 / d;
    *ampin1 = -(*ampin0 + *ampin0);
    *ampin2 = *ampin0;
    *ampout1 = (2 * (c2 - 1)) / d;
    *ampout2 = (1 - csqr2 + c2) / d;
}

void init_bandpass(float cutoff, float *ampin0, float *ampin1, float *ampin2, float *ampout1, float *ampout2) {
    float sqr2 = 1.414213562;
    float c = 1 / tanf((M_PI / SAMPLE_RATE) * cutoff);
    float d = 1 + c;
    
    *ampin0 = 1 / d;
    *ampin1 = 0;
    *ampin2 = -(*ampin0);
    *ampout1 = (-c*2*cosf((2*M_PI)*cutoff/SAMPLE_RATE)) / d;
    *ampout2 = (c - 1) / d;
}