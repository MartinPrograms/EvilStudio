#ifndef WAVEFORMGENERATOR_H
#define WAVEFORMGENERATOR_H
#include "audio/AudioGenerator.h"
#include "audio/piano.h"

namespace audio::Generators {

class WaveformGenerator final : public audio::AudioGenerator {
public:
    Waveform waveform = Waveform::Sine;
    float attack = 1.f; // Attack time in seconds
    float decay = 0.1f; // Decay time in seconds
    float sustain = 0.7f; // Sustain level (0.0 to 1.0)
    float release = 1.f; // Release time in seconds
    int unison = 1;
    float phase_randomization = 0.0f; // Phase randomization in radians

    WaveformGenerator()
        : AudioGenerator("Waveform Generator"){}

    void Process(float *buffer, int channels, int buffer_size, uint64_t current_sample) override;

private:
    void ProcessScheduledNoteOns(uint64_t sample_index, int read_idx);
    void ProcessScheduledNoteOffs(uint64_t sample_index, int read_idx);
};

}


#endif //WAVEFORMGENERATOR_H
