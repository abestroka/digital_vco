#include "envelope.h"
#include "pico/stdlib.h"
#include "i2s_audio.h"

void init_envelope(envelope_t *env, float attack_time, float decay_time, float sustain_level, float release_time) {
    env->state = OFF;
    env->value = 0.0f;
    env->attack_time = attack_time;
    env->decay_time = decay_time;
    env->sustain_level = sustain_level;
    env->release_time = release_time;
}

float process_envelope(envelope_t *env) {
    switch (env->state) {
        case ATTACK:
            env->value += (1.0f / (env->attack_time * SAMPLE_RATE));
            if (env->value >= 1.0f) {
                env->value = 1.0f;
                env->state = DECAY;
            }
            break;

        case DECAY:
            env->value -= (1.0f - env->sustain_level) / (env->decay_time * SAMPLE_RATE);
            if (env->value <= env->sustain_level) {
                env->value = env->sustain_level;
                env->state = SUSTAIN;
            }
            break;

        case SUSTAIN:
            env->value = env->sustain_level;
            break;

        case RELEASE:
            env->value -= env->sustain_level / (env->release_time * SAMPLE_RATE);
            if (env->value <= 0.0f) {
                env->value = 0.0f;
                env->state = OFF;
            }
            break;

        case OFF:
            env->value = 0.0f;
            break;
    }

    return env->value;
}

