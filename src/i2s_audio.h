#ifndef I2S_AUDIO_H
#define I2S_AUDIO_H

#define SAMPLE_RATE 44100

#include "pico/audio_i2s.h"

struct audio_buffer_pool *init_audio(void);

#endif // I2S_AUDIO_H
