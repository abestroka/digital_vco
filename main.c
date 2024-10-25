#include <stdio.h>
#include "pico/stdlib.h"
#include "i2s_audio.h"
#include "oscillators.h"
#include "envelope.h"

#include "hardware/adc.h"

#define ADC_PIN 26 
#define MAX_ADC_VALUE 4095.0f 

#define BUTTON_PIN 15
#define LED_PIN 25 
#define FREQUENCY 440.0f


float ampin0, ampin1, ampin2, ampout1, ampout2;


float read_potentiometer() {
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
        
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    adc_init();
    adc_gpio_init(ADC_PIN);  
    adc_select_input(0); 


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
    // float base_volume = 0.5f;

    envelope_t env;
    init_envelope(&env,0.01f, 0.01f, 1.0f, 0.01f); //ADSR

    // float min_volume = 0.0f;
    // float max_volume = 1.0f;    

    // static float smoothed_volume = 0;
    // float alpha = 0.1; // Smoothing factor (0 < alpha < 1)
    // float prev_adc_value = 0;

    float min_cutoff = 0.0f;
    float max_cutoff = 1000.0f;

    static float smoothed_cutoff = 0;
    float alpha = 0.1;
    float prev_adc_value = 0;

    // float cutoff_freq = 200.0f;

    // calculate_filter_coefficients(normalized_cutoff, &ampin0, &ampin1, &ampin2, &ampout1, &ampout2);
    float volume = 0.5;

    while (true) {
        // float pot_volume = read_potentiometer();
        // smoothed_volume = alpha * pot_volume + (1 - alpha) * smoothed_volume;
        // float base_volume = smoothed_volume - 0.2; 

        // float volume = base_volume;
        // if (volume < 0.01f) {
        //     volume = 0.0f;  // Silence for very low values
        // } else {
        //     volume = volume * volume;  // Apply a curve to improve volume control feel
        // }

        ////////////
        
        float pot_cutoff = read_potentiometer();
        smoothed_cutoff = alpha * pot_cutoff + (1 - alpha) * smoothed_cutoff;
        float base_cutoff = smoothed_cutoff;
        float cutoff_freq_p = base_cutoff;
        // if (cutoff_freq_p < 0.01f) {
        //     cutoff_freq_p = 0.0f;
        // } else {
        //     cutoff_freq_p = cutoff_freq_p * cutoff_freq_p;
        // }
        float cutoff_freq = cutoff_freq_p * max_cutoff;
        init_bandpass(cutoff_freq, &ampin0, &ampin1, &ampin2, &ampout1, &ampout2);



        if (gpio_get(BUTTON_PIN) == 0) {
            gpio_put(LED_PIN, 1); 
            if (env.state == OFF) {
                env.state = ATTACK;
            }
            play(audio_pool, current_wave_table, &pos, phase_inc, volume, &env, ampin0, ampin1, ampin2, ampout1, ampout2);
        } else {
            gpio_put(LED_PIN, 0);
            if (env.state != OFF && env.state != RELEASE) {
                env.state = RELEASE; 
            }
            if (env.state != OFF) {
                play(audio_pool, current_wave_table, &pos, phase_inc, volume, &env, ampin0, ampin1, ampin2, ampout1, ampout2);
            }
        }

    }

    return 0;
}
