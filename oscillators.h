#ifndef OSCILLATORS_H
#define OSCILLATORS_H

#include <stdint.h>
#include "pico/audio_i2s.h"
#include "envelope.h"

#define WAVE_TABLE_LEN 2048


extern float sine_wave_table[WAVE_TABLE_LEN];
extern float saw_wave_table[WAVE_TABLE_LEN];
extern float square_wave_table[WAVE_TABLE_LEN];
extern float triangle_wave_table[WAVE_TABLE_LEN];

void generate_sine_wave_table();
void generate_saw_wave_table();
void generate_square_wave_table();
void generate_triangle_wave_table();

void calculate_filter_coefficients(float cutoff_freq, float *ampin0, float *ampin1, float *ampin2, float *ampout1, float *ampout2);


void fill_audio_buffer(float *wave_table, int16_t *samples, uint32_t *pos, float phase_inc, float volume, int num_samples, envelope_t *env, float ampin0, float ampin1, float ampin2, float ampout1, float ampout2);
#endif // OSCILLATORS_H