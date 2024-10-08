#include "oscillators.h"
#include <math.h>
#include "i2s_audio.h"

float sine_wave_table[WAVE_TABLE_LEN];
float saw_wave_table[WAVE_TABLE_LEN];
float square_wave_table[WAVE_TABLE_LEN];
float triangle_wave_table[WAVE_TABLE_LEN];

// Envelope parameters 
#define ATTACK_TIME_SECONDS 0.1f  // 100 ms attack
#define DECAY_TIME_SECONDS 0.3f   // 300 ms decay
// #define SUSTAIN_LEVEL
#define RELEASE_TIME_SECONDS 0.5f

// Global variables to track envelope progress
static uint32_t envelope_pos = 0;  // Track the current envelope sample index


// Generate sine wave table
void generate_sine_wave_table() {
    for (int i = 0; i < WAVE_TABLE_LEN; i++) {
        sine_wave_table[i] = sinf(2.0f * M_PI * (float)i / (float)WAVE_TABLE_LEN);
    }
}

// Generate saw wave table
void generate_saw_wave_table() {
    for (int i = 0; i < WAVE_TABLE_LEN; i++) {
        saw_wave_table[i] = 2.0f * ((float)i / (float)WAVE_TABLE_LEN) - 1.0f; // Linearly interpolate between -1 and 1
    }
}

// Generate square wave table
void generate_square_wave_table() {
    for (int i = 0; i < WAVE_TABLE_LEN; i++) {
        if (i < WAVE_TABLE_LEN / 2) {
            square_wave_table[i] = 1.0f;
        } else {
            square_wave_table[i] = -1.0f;
        }
    }
}

// Generate triangle wave table
void generate_triangle_wave_table() {
    for (int i = 0; i < WAVE_TABLE_LEN; i++) {
        if (i < WAVE_TABLE_LEN / 2) {
            triangle_wave_table[i] = 4.0f * (float)i / WAVE_TABLE_LEN - 1.0f;
        } else {
            triangle_wave_table[i] = 3.0f - 4.0f * (float)i / WAVE_TABLE_LEN;
        }
    }
}

// Fill audio buffer with samples from wave table
void fill_audio_buffer(float *wave_table, int16_t *samples, uint32_t *pos, float phase_inc, float volume, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        // Get current sample from wave table using fractional indexing
        uint32_t index = (uint32_t)*pos;
        float sample = wave_table[index % WAVE_TABLE_LEN];

        // Apply volume and convert to int16 range (-32768 to 32767)
        samples[i] = (int16_t)(sample * volume * 32767.0f);

        // Increment phase and ensure it wraps around
        *pos += phase_inc;
        if (*pos >= WAVE_TABLE_LEN) {
            *pos -= WAVE_TABLE_LEN;
        }
    }
}


