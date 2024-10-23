#include "oscillators.h"
#include <math.h>
#include "i2s_audio.h"
#include "envelope.h"
float sine_wave_table[WAVE_TABLE_LEN];
float saw_wave_table[WAVE_TABLE_LEN];
float square_wave_table[WAVE_TABLE_LEN];
float triangle_wave_table[WAVE_TABLE_LEN];

// envelope params 
#define ATTACK_TIME_SECONDS 0.1f  // 100 ms attack
#define DECAY_TIME_SECONDS 0.3f   // 300 ms decay
// #define SUSTAIN_LEVEL
#define RELEASE_TIME_SECONDS 0.5f

// variables to track envelope progress
static uint32_t envelope_pos = 0;  // current envelope index


// sine wave table
void generate_sine_wave_table() {
    for (int i = 0; i < WAVE_TABLE_LEN; i++) {
        sine_wave_table[i] = sinf(2.0f * M_PI * (float)i / (float)WAVE_TABLE_LEN);
    }
}

// saw wave table
void generate_saw_wave_table() {
    for (int i = 0; i < WAVE_TABLE_LEN; i++) {
        saw_wave_table[i] = 2.0f * ((float)i / (float)WAVE_TABLE_LEN) - 1.0f; 
    }
}

// square wave table
void generate_square_wave_table() {
    for (int i = 0; i < WAVE_TABLE_LEN; i++) {
        if (i < WAVE_TABLE_LEN / 2) {
            square_wave_table[i] = 1.0f;
        } else {
            square_wave_table[i] = -1.0f;
        }
    }
}

// triangle wave table
void generate_triangle_wave_table() {
    for (int i = 0; i < WAVE_TABLE_LEN; i++) {
        if (i < WAVE_TABLE_LEN / 2) {
            triangle_wave_table[i] = 4.0f * (float)i / WAVE_TABLE_LEN - 1.0f;
        } else {
            triangle_wave_table[i] = 3.0f - 4.0f * (float)i / WAVE_TABLE_LEN;
        }
    }
}

static float dlyin1 = 0.0f;
static float dlyin2 = 0.0f;
static float dlyout1 = 0.0f;
static float dlyout2 = 0.0f;

void calculate_filter_coefficients(float cutoff, float *ampin0, float *ampin1, float *ampin2, float *ampout1, float *ampout2) {
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

// fill audio buffer with samples from wave table
void fill_audio_buffer(float *wave_table, int16_t *samples, uint32_t *pos, float phase_inc, float volume, int num_samples, envelope_t *env, float ampin0, float ampin1, float ampin2, float ampout1, float ampout2) {
    for (int i = 0; i < num_samples; i++) {
        // get envelope value
        float env_value = process_envelope(env);
        // get curent sample
        uint32_t index = (uint32_t)*pos;
        float sample = wave_table[index % WAVE_TABLE_LEN];

        float curr_volume = volume * env_value;

        float filtered_sample = (ampin0 * sample) + (ampin1 * dlyin1) + (ampin2 * dlyin2) - (ampout1 * dlyout1) - (ampout2 * dlyout2);

        // volume in int16 range -32768 to 32767
        // samples[i] = (int16_t)(sample * curr_volume * 32767.0f);
        samples[i] = (int16_t)(filtered_sample * curr_volume * 32767.0f);

        dlyout2 = dlyout1;
        dlyout1 = filtered_sample;
        dlyin2 = dlyin1;
        dlyin1 = sample;


        // next phase
        *pos += phase_inc;
        if (*pos >= WAVE_TABLE_LEN) {
            *pos -= WAVE_TABLE_LEN;
        }
    }
}


