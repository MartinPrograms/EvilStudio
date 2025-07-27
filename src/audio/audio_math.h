#ifndef AUDIO_MATH_H
#define AUDIO_MATH_H

#include <array>
#include <cmath>
#include <iostream>
#include "AudioDefinitions.h"

namespace audio {
namespace math {

inline float noise_buffer[NOISE_SAMPLES];

static void init_noise() {
    for (int i = 0; i < NOISE_SAMPLES; i++) {
        noise_buffer[i] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f; // Generate noise in range [-1, 1]
    }
}

static inline std::array<float,2> pan(const std::array<float,2>& in, float pan)
{
    float left_gain  = (1.0f - pan) * 0.5f;
    float right_gain = (1.0f + pan) * 0.5f;
    return { in[0] * left_gain,
             in[1] * right_gain };
}

static inline float generate_waveform(Waveform waveform, float phase)
{
    // First, fold phase into [0, 2π]
    float phase_mod = fmodf(phase, 2.0f * M_PI);
    if (phase_mod < 0.0f)
        phase_mod += 2.0f * M_PI;

    float result = 0.0f;
    switch (waveform)
    {
        case Waveform::Sine:
            // standard sine wave
            result = sinf(phase);
            break;

        case Waveform::Square:
            // +1 for first half‑cycle, –1 for second half
            result = (phase_mod < M_PI) ?  1.0f : -1.0f;
            break;

        case Waveform::Saw:
            // ramp from –1 at phase=0 to +1 at phase=2π
            result = (phase_mod / M_PI) - 1.0f;
            break;

        case Waveform::Triangle:
            // triangle via arcsine of sine
            result = (2.0f / M_PI) * asinf(sinf(phase));
            break;

        case Waveform::Noise:
        {
            result = noise_buffer[static_cast<int>(phase_mod * NOISE_SAMPLES / (2.0f * M_PI)) % NOISE_SAMPLES];
            break;
        }

        default:
            // safety fallback
            result = 0.0f;
            break;
    }

    return result;
}

}
}
#endif //AUDIO_MATH_H
