#include "sine_wave.h"
#include <math.h>
#include "pico/audio_i2s.h"

int16_t sine_wave_table[SINE_WAVE_TABLE_LEN];

void generate_sine_wave() {
    for (int i = 0; i < SINE_WAVE_TABLE_LEN; i++) {
        sine_wave_table[i] = 32767 * cosf(i * 2 * (float) (M_PI / SINE_WAVE_TABLE_LEN));
    }
}

void fill_audio_buffer(struct audio_buffer_pool *ap, uint32_t *pos, uint32_t step, uint8_t volume) {
    struct audio_buffer *buffer = take_audio_buffer(ap, true);
    int16_t *samples = (int16_t *) buffer->buffer->bytes;
    uint32_t pos_max = 0x10000 * SINE_WAVE_TABLE_LEN;

    for (uint32_t i = 0; i < buffer->max_sample_count; i++) {
        samples[i] = (volume * sine_wave_table[*pos >> 16u]) >> 8u;
        *pos += step;
        if (*pos >= pos_max) {
            *pos -= pos_max;
        }
    }

    buffer->sample_count = buffer->max_sample_count;
    give_audio_buffer(ap, buffer);
}
