#ifndef OSCILLATORS_H
#define OSCILLATORS_H

#include <stdint.h>
#include "pico/audio_i2s.h"

#define WAVE_TABLE_LEN 2048

// Enum to select waveform type
typedef enum {
    WAVEFORM_SINE,
    WAVEFORM_SAW,
    WAVEFORM_SQUARE,
    WAVEFORM_TRIANGLE
} waveform_t;

extern int16_t sine_wave_table[WAVE_TABLE_LEN];
extern int16_t saw_wave_table[WAVE_TABLE_LEN];
extern int16_t square_wave_table[WAVE_TABLE_LEN];
extern int16_t triangle_wave_table[WAVE_TABLE_LEN];

// Functions to generate different waveforms
void generate_waveform(waveform_t waveform_type);
void fill_audio_buffer(struct audio_buffer_pool *ap, uint32_t *pos, uint32_t step, uint8_t volume, waveform_t waveform_type);

#endif // OSCILLATORS_H