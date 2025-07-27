#include "WaveformGenerator.h"

#include <algorithm>
#include <math.h>

#include "audio/AudioBackend.h"
#include "audio/audio_math.h"

namespace audio::Generators {
    void WaveformGenerator::ProcessScheduledNoteOns(uint64_t sample_index, int read_idx) {
        auto& scheduled_on = scheduled_note_buffer_on[read_idx];
        if (!scheduled_on.empty()) {
            for (auto& note : scheduled_on) {
                int note_number = (note.note_number);
                int velocity = note.velocity;
                float frequency = audio::piano::midi_to_frequency(note_number);
                float amplitude = static_cast<float>(velocity) / 127.0f; // Normalize velocity to [0, 1]

                const float detune = 0.5f; // Detune in semitones
                for (int i = 0; i < unison; ++i) {
                    Sequencing::Voice voice{};
                    float detune_cents = (i - (unison - 1) / 2.0f) * detune; // detune in cents
                    float detune_ratio = std::pow(2.0f, detune_cents / 1200.0f); // convert cents to frequency ratio
                    voice.frequency = frequency * detune_ratio;

                    voice.amplitude = amplitude / static_cast<float>(unison); // normalize amplitude to prevent volume overload

                    // Pan across stereo field for each unison voice
                    if (unison > 1) {
                        voice.pan = (i / static_cast<float>(unison - 1)) * 2.0f - 1.0f; // spread from -1.0 (L) to +1.0 (R)
                    } else {
                        voice.pan = 0.0f;
                    }

                    // Optional phase randomization
                    if (phase_randomization > 0.0f) {
                        float random_phase = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * phase_randomization;
                        voice.phase = random_phase;
                    } else {
                        voice.phase = 0.0f;
                    }

                    voice.id = static_cast<int>(note_number);
                    voice.creation_time = sample_index;

                    voice.envelope.attackTime = static_cast<uint64_t>(attack * SAMPLE_RATE);
                    voice.envelope.attackTension = 0.5f;
                    voice.envelope.decayTime = static_cast<uint64_t>(decay * SAMPLE_RATE);
                    voice.envelope.decayTension = 0.5f;
                    voice.envelope.sustainLevel = sustain;
                    voice.envelope.releaseTime = static_cast<uint64_t>(release * SAMPLE_RATE);
                    voice.envelope.releaseTension = 0.5f;

                    this->voices.push_back(voice);
                }
            }
            scheduled_on.clear(); // Clear scheduled notes after processing
        }
    }

    void WaveformGenerator::ProcessScheduledNoteOffs(uint64_t sample_index, int read_idx) {
        auto& scheduled_off = scheduled_note_buffer_off[read_idx];
        if (!scheduled_off.empty()) {
            for (auto& note_off : scheduled_off) {
                int note = note_off.note_number;

                for (auto& voice : this->voices) {
                    if (voice.id == note) {
                        voice.envelope.state = Sequencing::AdsrState::Release; // Set envelope to release state
                        voice.creation_time = sample_index; // Update creation time to current sample index

                        voice.envelope.enterRelease(sample_index);
                    }
                }
            }
            scheduled_off.clear();
        }
    }

    void WaveformGenerator::Process(float *buffer, int channels, int buffer_size, uint64_t current_sample) {
        const float two_pi = 2.0f * float(M_PI);
        const float sample_rate_inv = 1.0f / SAMPLE_RATE;

        int read_idx = write_buffer_index.exchange(1 - write_buffer_index); // atomically swap

        ProcessScheduledNoteOns(current_sample, read_idx);
        ProcessScheduledNoteOffs(current_sample, read_idx);

        std::vector<Sequencing::Voice*> to_remove;

        for (int f = 0; f < buffer_size; ++f) {
            uint64_t sample_index = current_sample + f;

            for (auto& voice : this->voices) {
                const float phaseInc = two_pi * voice.frequency / SAMPLE_RATE;
                voice.phase += phaseInc;
                if (voice.phase >= two_pi) voice.phase -= two_pi;
                float s = audio::math::generate_waveform(waveform, voice.phase) * voice.amplitude * volume;
                int idx = f * channels;

                bool to_delete = false; // This is to avoid unused variable warnings
                float gh = voice.envelope.process(sample_index, voice.creation_time, &to_delete);
                s *= gh; // Apply envelope to the sample

                std::array sample = {s, s};

                std::array<float, 2> panned = audio::math::pan(sample, voice.pan);
                std::array<float, 2> panned_master = audio::math::pan(panned, pan); // Use the generator's pan
                buffer[idx] += panned_master[0];
                if (channels > 1)
                    buffer[idx + 1] += panned_master[1];

                if (to_delete) {
                    to_remove.push_back(&voice); // Mark voice for removal
                }
            }
        }

        // Remove voices marked for deletion
        if (!to_remove.empty()) {
            voices.erase(
                std::remove_if(voices.begin(), voices.end(),
                    [&to_remove](const Sequencing::Voice& v) {
                        return std::any_of(to_remove.begin(), to_remove.end(),
                                           [&v](const Sequencing::Voice* ptr) { return ptr == &v; });
                    }),
                voices.end()
            );
        }
    }
}
