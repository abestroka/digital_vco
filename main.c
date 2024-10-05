#include <stdio.h>
#include "pico/stdlib.h"
#include "i2s_audio.h"
#include "sine_wave.h"

#define BUTTON_PIN 15
#define LED_PIN 25 


int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
        
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Generate the sine wave lookup table
    generate_sine_wave();

    // Initialize I2S audio
    struct audio_buffer_pool *ap = init_audio();

    // Step defines the frequency
    uint32_t step = 0x200000;
    uint32_t pos = 0;
    uint8_t volume = 128;

    while (true) {

        if (gpio_get(BUTTON_PIN) == 0) {
            gpio_put(LED_PIN, 1); //LED on
            play_sine_wave(ap, &pos, step, volume);
        } else {
            gpio_put(LED_PIN, 0); // LED off
        }

        // sleep_ms(20);

    }

    return 0;
}
