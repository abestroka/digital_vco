#ifndef FILTERS_H
#define FILTERS_H

typedef struct {
    float cutoff_freq;  
    float resonance;    
    float sample_rate;  

    float prev_input;
    float prev_output;
} filter_t;

void init_filter(filter_t *filter, float cutoff_freq, float resonance, float sample_rate);

float apply_lowpass(filter_t *filter, float sample);

float apply_highpass(filter_t *filter, float sample);

float apply_bandpass(filter_t *filter, float sample);

#endif
