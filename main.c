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
#define FREQUENCY1 60.0f
#define FREQUENCY2 400.0f

// TODO: simultaneous button release causes clipping

float ampin0, ampin1, ampin2, ampout1, ampout2;

typedef struct {
    float *wave_table;
    uint32_t *pos;
    float phase_inc;
    float volume;
    envelope_t env;
    filter_state_t *filter_state;
    bool is_active;
} voice;


void apply_smoothing(int16_t *samples, int sample_count, float alpha) {
    float smoothed = samples[0]; 

    for (int i = 1; i < sample_count; i++) {
        smoothed = alpha * samples[i] + (1 - alpha) * smoothed;
        samples[i] = (int16_t)smoothed;
    }
}

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

void play(struct audio_buffer_pool *audio_pool, voice **voices, int num_voices, 
          float ampin0, float ampin1, float ampin2, float ampout1, float ampout2) {
    struct audio_buffer *buffer = take_audio_buffer(audio_pool, true);
    int16_t *samples = (int16_t *) buffer->buffer->bytes;

    // clear buffer
    for (int i = 0; i < buffer->max_sample_count; i++) {
        samples[i] = 0;
    }

    // loop through all active voices and fill buffer
    for (int v = 0; v < num_voices; v++) {
        if (voices[v]->is_active == true) {
            fill_audio_buffer(voices[v]->wave_table, samples, voices[v]->pos, voices[v]->phase_inc, 
                            voices[v]->volume, buffer->max_sample_count, &voices[v]->env, 
                            ampin0, ampin1, ampin2, ampout1, ampout2, voices[v]->filter_state);
        }
    }

    apply_smoothing(samples, buffer->max_sample_count, 0.1f);

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

    uint32_t pos1 = 0;
    filter_state_t filter_state1 = {0};

    //initialize voice struct
    voice voice1 = {0};
    voice1.wave_table = sine_wave_table;
    voice1.pos = &pos1;
    voice1.phase_inc = (WAVE_TABLE_LEN * FREQUENCY1) / SAMPLE_RATE;
    voice1.volume = 0.5;
    init_envelope(&voice1.env, 0.01f, 0.1f, 1.0f, 0.5f); //ADSR
    voice1.filter_state = &filter_state1;
    voice1.is_active = false;

    uint32_t pos2 = 0;
    filter_state_t filter_state2 = {0};

    voice voice2 = {0};
    voice2.wave_table = white_noise_table;
    voice2.pos = &pos2;
    voice2.phase_inc = (WAVE_TABLE_LEN * FREQUENCY2) / SAMPLE_RATE;
    voice2.volume = 0.2;
    init_envelope(&voice2.env, 0.01f, 0.1f, 1.0f, 0.3f); //ADSR
    voice2.filter_state = &filter_state2;
    voice2.is_active = false;

    voice *voices[2];
    voices[0] = &voice1;
    voices[1] = &voice2;


    float min_volume = 0.0f;
    float max_volume = 1.0f;    
    static float smoothed_volume = 0;

    float min_cutoff = 0.0f;
    float max_cutoff = FREQUENCY2;

    static float smoothed_cutoff = 0;
    float alpha = 0.1;

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
            // if (voice1.env.state == OFF) {
            voice1.env.state = ATTACK;
            // }
            voice1.is_active = true;
            play(audio_pool, voices, 2, ampin0, ampin1, ampin2, ampout1, ampout2);
        } else if (gpio_get(BUTTON_PIN_1) == 1) {
            gpio_put(LED_PIN, 0);
            if (voice1.env.state != OFF && voice1.env.state != RELEASE) {
                voice1.env.state = RELEASE; 
            }
            if (voice1.env.state != OFF) {
                play(audio_pool, voices, 2, ampin0, ampin1, ampin2, ampout1, ampout2);
            } else {
                voice1.is_active = false;
            }
        }
        if (gpio_get(BUTTON_PIN_2) == 0) {
            gpio_put(LED_PIN, 1); 
            // if (voice2.env.state == OFF) {
            voice2.env.state = ATTACK;
            // }
            voice2.is_active = true;
            play(audio_pool, voices, 2, ampin0, ampin1, ampin2, ampout1, ampout2);
        } else if (gpio_get(BUTTON_PIN_2) == 1) {
            gpio_put(LED_PIN, 0);
            if (voice2.env.state != OFF && voice2.env.state != RELEASE) {
                voice2.env.state = RELEASE; 
            }
            if (voice2.env.state != OFF) {
                play(audio_pool, voices, 2, ampin0, ampin1, ampin2, ampout1, ampout2);
            } else {
                voice2.is_active = false;
            }
        }
    }

    return 0;
}
