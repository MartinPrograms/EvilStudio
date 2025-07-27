#ifndef AUDIODEFINITIONS_H
#define AUDIODEFINITIONS_H

#include <array>

#define SAMPLE_RATE 44100
#define NOISE_SAMPLES SAMPLE_RATE
#define BUFFER_SIZE 1024
#define SLICE_SIZE SAMPLE_RATE

namespace audio {
    enum class Waveform {
        Sine,
        Square,
        Saw,
        Triangle,
        Noise
    };

    namespace waveform {
        constexpr std::array<Waveform, 5> all_waveforms = {
            Waveform::Sine,
            Waveform::Square,
            Waveform::Saw,
            Waveform::Triangle,
            Waveform::Noise
        };

        constexpr const char* to_string(Waveform wf) {
            switch (wf) {
                case Waveform::Sine:     return "Sine";
                case Waveform::Square:   return "Square";
                case Waveform::Saw:      return "Saw";
                case Waveform::Triangle: return "Triangle";
                case Waveform::Noise:    return "Noise";
                default:                 return "Unknown";
            }
        }
    }
}

#endif //AUDIODEFINITIONS_H
