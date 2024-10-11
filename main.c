#include <stdio.h>
#include "pico/stdlib.h"
#include "i2s_audio.h"
#include "oscillators.h"
#include "envelope.h"
#include "filters.h"

#define BUTTON_PIN 15
#define LED_PIN 25 
#define FREQUENCY 440.0f

float attack_time = 0.1f;   
float decay_time = 0.2f;    
float sustain_level = 0.3f; 
float release_time = 0.3f; 


void play(struct audio_buffer_pool *audio_pool, float *wave_table, uint32_t *pos, float phase_inc, float volume, envelope_t *env) {
    struct audio_buffer *buffer = take_audio_buffer(audio_pool, true);
    int16_t *samples = (int16_t *) buffer->buffer->bytes;
    // uint8_t volume2 = volume * 0.1;
    fill_audio_buffer(wave_table, samples, pos, phase_inc, volume, buffer->max_sample_count, env);
    buffer->sample_count = buffer->max_sample_count;
    give_audio_buffer(audio_pool, buffer);
}




int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
        
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);


    // Initialize I2S
    struct audio_buffer_pool *audio_pool = init_audio();

    // generate all wave tables
    generate_sine_wave_table();
    generate_saw_wave_table();
    generate_square_wave_table();
    generate_triangle_wave_table();

    // choose wave table
    float *current_wave_table = sine_wave_table; 

    float phase_inc = (WAVE_TABLE_LEN * FREQUENCY) / SAMPLE_RATE;

    uint32_t pos = 0;
    float base_volume = 0.5f;

    envelope_t env;
    init_envelope(&env, 1.0f, 0.0f, 0.7f, 1.0f); //ADSR

    while (true) {

        if (gpio_get(BUTTON_PIN) == 0) {
            gpio_put(LED_PIN, 1); 
            if (env.state == OFF) {
                env.state = ATTACK;
            }
            play(audio_pool, current_wave_table, &pos, phase_inc, base_volume, &env);
        } else {
            gpio_put(LED_PIN, 0);
            if (env.state != OFF && env.state != RELEASE) {
                env.state = RELEASE; 
            }
            if (env.state != OFF) {
                play(audio_pool, current_wave_table, &pos, phase_inc, base_volume, &env);
            }
        }

    }

    return 0;
}
