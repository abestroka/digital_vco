#ifndef ENVELOPE_H
#define ENVELOPE_H

typedef enum { ATTACK, DECAY, SUSTAIN, RELEASE, OFF } envelope_state_t;

typedef struct {
    envelope_state_t state;
    float value;
    float attack_time;
    float decay_time;
    float sustain_level;
    float release_time;
} envelope_t;

void init_envelope(envelope_t *env, float attack_time, float decay_time, float sustain_level, float release_time);
float process_envelope(envelope_t *env);

#endif // ENVELOPE_H
