#include <stdio.h>
#include "pico/stdlib.h"
#include "i2s_audio.h"
#include "oscillators.h"
#include "envelope.h"
#include "filters.h"

#include "hardware/adc.h"

#define CUTOFF_ADC_CHANNEL 0
#define VOLUME_ADC_CHANNEL 1
#define CUTOFF_PIN 26
#define VOLUME_PIN 27
#define MAX_ADC_VALUE 4095.0f 

#define BUTTON_PIN_1 15
#define BUTTON_PIN_2 14
#define LED_PIN 25 
#define FREQUENCY 60.0f


float ampin0, ampin1, ampin2, ampout1, ampout2;


float read_cutoff() {
    adc_select_input(CUTOFF_ADC_CHANNEL);
    uint16_t result = adc_read();  //read ADC value (0-4095)
    return (result / MAX_ADC_VALUE);  // 0 - 1
    sleep_ms(10);
}

float read_volume() {
    adc_select_input(VOLUME_ADC_CHANNEL);
    uint16_t result = adc_read();  //read ADC value (0-4095)
    return (result / MAX_ADC_VALUE);  // 0 - 1
    sleep_ms(10);
}


void play(struct audio_buffer_pool *audio_pool, float *wave_table, uint32_t *pos, float phase_inc, float volume, envelope_t *env, float ampin0, float ampin1, float ampin2, float ampout1, float ampout2) {
    struct audio_buffer *buffer = take_audio_buffer(audio_pool, true);
    int16_t *samples = (int16_t *) buffer->buffer->bytes;
    // uint8_t volume2 = volume * 0.1;
    fill_audio_buffer(wave_table, samples, pos, phase_inc, volume, buffer->max_sample_count, env, ampin0, ampin1, ampin2, ampout1, ampout2);
    buffer->sample_count = buffer->max_sample_count;
    give_audio_buffer(audio_pool, buffer);
}


int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
        
    gpio_init(BUTTON_PIN_1);
    gpio_set_dir(BUTTON_PIN_1, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_1);

    gpio_init(BUTTON_PIN_2);
    gpio_set_dir(BUTTON_PIN_2, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_2);

    adc_init();
    adc_gpio_init(CUTOFF_PIN);  
    adc_gpio_init(VOLUME_ADC_CHANNEL);



    // Initialize I2S
    struct audio_buffer_pool *audio_pool = init_audio();

    // generate all wave tables
    generate_sine_wave_table();
    generate_saw_wave_table();
    generate_square_wave_table();
    generate_triangle_wave_table();
    generate_white_noise_table();

    // choose wave table
    // float *current_wave_table = sine_wave_table; 
    float *current_wave_table = sine_wave_table; 

    float phase_inc = (WAVE_TABLE_LEN * FREQUENCY) / SAMPLE_RATE;
    float phase_inc2 = (WAVE_TABLE_LEN * 200.0) / SAMPLE_RATE;

    uint32_t pos = 0;

    envelope_t env1, env2;
    init_envelope(&env1, 0.01f, 0.1f, 1.0f, 0.1f);
    init_envelope(&env2, 0.01f, 0.1f, 1.0f, 0.1f); //ADSR

    float min_volume = 0.0f;
    float max_volume = 1.0f;    
    static float smoothed_volume = 0;

    // float alpha = 0.1; // Smoothing factor (0 < alpha < 1)

    float min_cutoff = 0.0f;
    float max_cutoff = FREQUENCY;

    static float smoothed_cutoff = 0;
    float alpha = 0.1;

    // float cutoff_freq = 200.0f;
    float volume = 0.5;

    while (true) {
        // float pot_volume = read_volume();
        // smoothed_volume = alpha * pot_volume + (1 - alpha) * smoothed_volume;
        // float base_volume = smoothed_volume - 0.2; 

        // float volume = base_volume;
        // if (volume < 0.01f) {
        //     volume = 0.0f;  // Silence for very low values
        // } else {
        //     volume = volume * volume;  // Apply a curve to improve volume control feel
        // }

        ////////////
        
        float pot_cutoff = read_cutoff();
        smoothed_cutoff = alpha * pot_cutoff + (1 - alpha) * smoothed_cutoff;
        float base_cutoff = smoothed_cutoff;
        float cutoff_freq_p = base_cutoff;
        float cutoff_freq = cutoff_freq_p * max_cutoff;
        init_lowpass(cutoff_freq, &ampin0, &ampin1, &ampin2, &ampout1, &ampout2);



        if (gpio_get(BUTTON_PIN_1) == 0) {
            gpio_put(LED_PIN, 1); 
            if (env1.state == OFF) {
                env1.state = ATTACK;
            }
            play(audio_pool, current_wave_table, &pos, phase_inc, volume, &env1, ampin0, ampin1, ampin2, ampout1, ampout2);
        } else if (gpio_get(BUTTON_PIN_1) == 1) {
            gpio_put(LED_PIN, 0);
            if (env1.state != OFF && env1.state != RELEASE) {
                env1.state = RELEASE; 
            }
            if (env1.state != OFF) {
                play(audio_pool, current_wave_table, &pos, phase_inc, volume, &env1, ampin0, ampin1, ampin2, ampout1, ampout2);
            }
        }


        if (gpio_get(BUTTON_PIN_2) == 0) {
            gpio_put(LED_PIN, 1); 
            if (env2.state == OFF) {
                env2.state = ATTACK;
            }
            play(audio_pool, current_wave_table, &pos, phase_inc2, volume, &env2, ampin0, ampin1, ampin2, ampout1, ampout2);
        } else if (gpio_get(BUTTON_PIN_2) == 1) {
            gpio_put(LED_PIN, 0);
            if (env2.state != OFF && env2.state != RELEASE) {
                env2.state = RELEASE; 
            }
            if (env2.state != OFF) {
                play(audio_pool, current_wave_table, &pos, phase_inc2, volume, &env2, ampin0, ampin1, ampin2, ampout1, ampout2);
            }
        }

        

    }

    return 0;
}
