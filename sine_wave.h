#ifndef SINE_WAVE_H
#define SINE_WAVE_H

#include <stdint.h>
#include "pico/audio_i2s.h"

#define SINE_WAVE_TABLE_LEN 2048

extern int16_t sine_wave_table[SINE_WAVE_TABLE_LEN];

void generate_sine_wave(void);
void play_sine_wave(struct audio_buffer_pool *ap, uint32_t *pos, uint32_t step, uint8_t volume);

#endif // SINE_WAVE_H
