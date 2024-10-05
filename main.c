#include <stdio.h>
#include "pico/stdlib.h"
#include "i2s_audio.h"
#include "sine_wave.h"

int main() {
    stdio_init_all();

    // Generate the sine wave lookup table
    generate_sine_wave();

    // Initialize I2S audio
    struct audio_buffer_pool *ap = init_audio();

    // Step defines the frequency
    uint32_t step = 0x200000;
    uint32_t pos = 0;
    uint8_t volume = 128;

    while (true) {
        fill_audio_buffer(ap, &pos, step, volume);
    }

    return 0;
}
