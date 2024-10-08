#include <stdio.h>
#include "pico/stdlib.h"
#include "i2s_audio.h"
#include "oscillators.h"

#define BUTTON_PIN 15
#define LED_PIN 25 
#define FREQUENCY 440.0f



int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
        
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);


    // Initialize I2S audio
    struct audio_buffer_pool *audio_pool = init_audio();

    // Generate all wave tables
    generate_sine_wave_table();
    generate_saw_wave_table();
    generate_square_wave_table();
    generate_triangle_wave_table();

    // Select which wave table to use (e.g., sine, saw, square, triangle)
    float *current_wave_table = sine_wave_table; 

    float phase_inc = (WAVE_TABLE_LEN * FREQUENCY) / SAMPLE_RATE;

    uint32_t pos = 0;
    float volume = 0.2f;

    while (true) {



        if (gpio_get(BUTTON_PIN) == 0) {
            gpio_put(LED_PIN, 1); //LED on
            struct audio_buffer *buffer = take_audio_buffer(audio_pool, true);
            int16_t *samples = (int16_t *) buffer->buffer->bytes;
            // uint8_t volume2 = volume * 0.1;
            fill_audio_buffer(current_wave_table, samples, &pos, phase_inc, volume, buffer->max_sample_count);
            buffer->sample_count = buffer->max_sample_count;
            give_audio_buffer(audio_pool, buffer);
        } else {
            gpio_put(LED_PIN, 0); // LED off
        }

        // Fill the buffer with audio samples from the selected wave table
        // fill_audio_buffer(current_wave_table, samples, &pos, phase_inc, volume, buffer->max_sample_count);

        // buffer->sample_count = buffer->max_sample_count;
        // give_audio_buffer(audio_pool, buffer);

        // sleep_ms(20);

    }

    return 0;
}
