#include "filters.h"
#include <math.h>

#define PI 3.14159265358979323846

void init_filter(filter_t *filter, float cutoff_freq, float resonance, float sample_rate) {
    filter->cutoff_freq = cutoff_freq;
    filter->resonance = resonance;
    filter->sample_rate = sample_rate;
    filter->prev_input = 0.0f;
    filter->prev_output = 0.0f;
}

float apply_lowpass(filter_t *filter, float sample) {
    float c = 1.0f / tan(PI * filter->cutoff_freq / filter->sample_rate);
    float a1 = 1.0f / (1.0f + filter->resonance * c + c * c);
    float a2 = 2.0f * a1;
    float a3 = a1;
    float b1 = 2.0f * (1.0f - c * c) * a1;
    float b2 = (1.0f - filter->resonance * c + c * c) * a1;

    float output = a1 * sample + a2 * filter->prev_input + a3 * filter->prev_output 
                   - b1 * filter->prev_input - b2 * filter->prev_output;

    
    filter->prev_input = sample;
    filter->prev_output = output;

    return output;
}

float apply_highpass(filter_t *filter, float sample) {
    float c = tan(PI * filter->cutoff_freq / filter->sample_rate);
    float a1 = 1.0f / (1.0f + filter->resonance * c + c * c);
    float a2 = -2.0f * a1;
    float a3 = a1;
    float b1 = 2.0f * (c * c - 1.0f) * a1;
    float b2 = (1.0f - filter->resonance * c + c * c) * a1;

    float output = a1 * sample + a2 * filter->prev_input + a3 * filter->prev_output 
                   - b1 * filter->prev_input - b2 * filter->prev_output;

    filter->prev_input = sample;
    filter->prev_output = output;

    return output;
}

float apply_bandpass(filter_t *filter, float sample) {
    float c = 1.0f / tan(PI * filter->cutoff_freq / filter->sample_rate);
    float a1 = filter->resonance * c / (1.0f + c);
    float b1 = (1.0f - c) / (1.0f + c);

    float output = a1 * (sample - filter->prev_input) + b1 * filter->prev_output;


    filter->prev_input = sample;
    filter->prev_output = output;

    return output;
}
