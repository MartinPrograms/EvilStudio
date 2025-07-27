#ifndef VOICE_H
#define VOICE_H
#include <cstdint>

namespace audio::Sequencing {

    enum class AdsrState{
        Attack,     // In attack phase
        Decay,      // In decay phase
        Sustain,    // In sustain phase
        Release     // In release phase
    };

    struct AdsrEnvelope {
        AdsrState state = AdsrState::Attack;

        uint64_t attackTime;
        float attackTension;

        uint64_t decayTime;
        float decayTension;

        float sustainLevel;

        uint64_t releaseTime;
        float releaseTension;

        float currentAmplitude = 0.0f;

        // New field to track amplitude at start of release
        float releaseStartAmplitude = 0.0f;

        float process(uint64_t current_sample, uint64_t creation_time, bool* should_kill) {
            switch (state) {
                case AdsrState::Attack: {
                    if (current_sample >= creation_time + attackTime) {
                        state = AdsrState::Decay;
                        currentAmplitude = 1.0f;
                        return 1.0f;
                    }
                    float t = static_cast<float>(current_sample - creation_time) / attackTime;
                    float a = lerp_tension(0.0f, 1.0f, t, attackTension);
                    currentAmplitude = a;
                    return a;
                }
                case AdsrState::Decay: {
                    if (current_sample >= creation_time + attackTime + decayTime) {
                        state = AdsrState::Sustain;
                        currentAmplitude = sustainLevel;
                        return sustainLevel;
                    }
                    float t = static_cast<float>(current_sample - (creation_time + attackTime)) / decayTime;
                    float a = lerp_tension(1.0f, sustainLevel, t, decayTension);
                    currentAmplitude = a;
                    return a;
                }
                case AdsrState::Sustain: {
                    currentAmplitude = sustainLevel;
                    return sustainLevel;
                }
                case AdsrState::Release: {
                    if (current_sample >= creation_time + releaseTime) {
                        *should_kill = true;
                        currentAmplitude = 0.0f;
                        return 0.0f;
                    }
                    float t = static_cast<float>(current_sample - creation_time) / releaseTime;
                    float a = lerp_tension(releaseStartAmplitude, 0.0f, t, releaseTension);
                    currentAmplitude = a;
                    return a;
                }
            }

            return 0.0f;
        }

        void enterRelease(uint64_t current_sample) {
            releaseStartAmplitude = currentAmplitude;
            state = AdsrState::Release;
        }

    private:
        static float lerp_tension(float a, float b, float t, float tension) {
            return a + (b - a) * t;
        }
    };

    struct Voice {
        float frequency{};
        float amplitude{};
        float detune{};
        float phase{};
        float pan{};
        int id{}; // Unique identifier for the voice, can be used to track it in a collection
        uint64_t creation_time{}; // Time when the voice was created
        AdsrEnvelope envelope {};
    };
}

#endif //VOICE_H
