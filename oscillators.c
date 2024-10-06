#include "oscillators.h"
#include <math.h>

int16_t sine_wave_table[WAVE_TABLE_LEN];
int16_t saw_wave_table[WAVE_TABLE_LEN];
int16_t square_wave_table[WAVE_TABLE_LEN];
int16_t triangle_wave_table[WAVE_TABLE_LEN];

// Generate the selected waveform
void generate_waveform(waveform_t waveform_type) {
    switch (waveform_type) {
        case WAVEFORM_SINE:
            for (int i = 0; i < WAVE_TABLE_LEN; i++) {
                sine_wave_table[i] = 32767 * cosf(i * 2 * (float) M_PI / WAVE_TABLE_LEN);
            }
            break;
        case WAVEFORM_SAW:
            for (int i = 0; i < WAVE_TABLE_LEN; i++) {
                saw_wave_table[i] = (int16_t)(65535 * ((float)i / WAVE_TABLE_LEN) - 32768);
            }
            break;
        case WAVEFORM_SQUARE:
            for (int i = 0; i < WAVE_TABLE_LEN; i++) {
                square_wave_table[i] = (i < WAVE_TABLE_LEN / 2) ? 32767 : -32768;
            }
            break;
        case WAVEFORM_TRIANGLE:
            for (int i = 0; i < WAVE_TABLE_LEN / 2; i++) {
                triangle_wave_table[i] = (int16_t)(65535 * ((float)i / (WAVE_TABLE_LEN / 2)) - 32768);
            }
            for (int i = WAVE_TABLE_LEN / 2; i < WAVE_TABLE_LEN; i++) {
                triangle_wave_table[i] = (int16_t)(65535 * (1.0f - (float)(i - WAVE_TABLE_LEN / 2) / (WAVE_TABLE_LEN / 2)) - 32768);
            }
            break;
    }
}

void fill_audio_buffer(struct audio_buffer_pool *ap, uint32_t *pos, uint32_t step, uint8_t volume, waveform_t waveform_type) {
    struct audio_buffer *buffer = take_audio_buffer(ap, true);
    int16_t *samples = (int16_t *) buffer->buffer->bytes;
    uint32_t pos_max = 0x10000 * WAVE_TABLE_LEN;

    int16_t *wave_table;

    // Choose the correct waveform table based on waveform_type
    switch (waveform_type) {
        case WAVEFORM_SINE:
            wave_table = sine_wave_table;
            break;
        case WAVEFORM_SAW:
            wave_table = saw_wave_table;
            break;
        case WAVEFORM_SQUARE:
            wave_table = square_wave_table;
            break;
        case WAVEFORM_TRIANGLE:
            wave_table = triangle_wave_table;
            break;
        default:
            wave_table = sine_wave_table; // Default to sine wave
            break;
    }
    // attack_time = attack_rate * 

    for (uint32_t i = 0; i < buffer->max_sample_count; i++) {
        samples[i] = (volume * wave_table[*pos >> 16u]) >> 8u;
        *pos += step;
        if (*pos >= pos_max) {
            *pos -= pos_max;
        }
    }

    buffer->sample_count = buffer->max_sample_count;
    give_audio_buffer(ap, buffer);
}
