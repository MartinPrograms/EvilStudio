#include "AudioBackend.h"

#include <cmath>
#include <cstring>
#include <memory>
#include <stdexcept>

#include "audio_math.h"
#include "Generators/WaveformGenerator.h"

alignas(32) float buffer[BUFFER_SIZE * 2];

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    auto* out = static_cast<float*>(pOutput);
    int frames   = static_cast<int>(frameCount);
    int floats   = frames * 2;
    std::fill(buffer, buffer + floats, 0.0f);

    for (auto& gen : audio::AudioBackend::audio_backend->generators) {
        gen->Process(buffer, 2, frames, audio::AudioBackend::audio_backend->sequencer_state.get_current_process_sample()); // Still process even if the sequencer is not playing, this simply means no new note events will be generated
    }

    auto pan = audio::AudioBackend::audio_backend->master_pan;
    for (int i = 0; i < floats; i+=2) {
        auto panned = audio::math::pan({buffer[i], buffer[i+1]}, pan);
        buffer[i] = panned[0];
        buffer[i+1] = panned[1];
    }

    if (audio::AudioBackend::audio_backend->sequencer_state.is_playing_state()) {
        audio::AudioBackend::audio_backend->sequencer_state.move_forward(frames);
    }

    audio::AudioBackend::audio_backend->sequencer_state.processed(frames);


    auto volume = audio::AudioBackend::audio_backend->master_volume;
    for (int i = 0; i < floats; ++i) {
        out[i] = buffer[i] * volume;
    }
}

namespace audio {
    AudioBackend* AudioBackend::audio_backend = nullptr;

    AudioBackend::AudioBackend() {
        if (audio_backend != nullptr) {
            throw std::runtime_error("Audio backend already instantiated!!");
        }

        audio_backend = this;

        ma_device_config config = ma_device_config_init(ma_device_type_playback);
        config.playback.format = ma_format_f32;
        config.playback.channels = 2;
        config.sampleRate = SAMPLE_RATE;
        config.dataCallback = data_callback;

        if (ma_device_init(nullptr, &config, &device) != MA_SUCCESS) {
            throw std::runtime_error("Failed to initialise audio device!!");
        }

        if (ma_device_start(&device) != MA_SUCCESS) {
            ma_device_uninit(&device);
            throw std::runtime_error("Failed to start device!!");
        }

        audio::math::init_noise();

        // TODO: Remove VVVVVVV
        generators.push_back(new Generators::WaveformGenerator());

        sequencer_state.reset();

        sequencer_state.reset_callbacks.emplace_back([] {
            for (auto& gen : AudioBackend::audio_backend->generators) {
                gen->voices.clear();
            }
        });

        midi_manager.note_callbacks.emplace_back([this](int note, int velocity) {
            if (AudioBackend::audio_backend->selected_generator == -1) {
                return;
            }

            auto generator = generators[selected_generator];
            if (generator) {
                if (velocity > 0) {
                    generator->NoteOn({note, velocity});
                }
                else {
                    generator->NoteOff({note, 0}); // NoteOff with velocity 0
                }
            } else {
                std::cerr << "No generator selected or generator is null!" << std::endl;
            }
        });

        midi_manager.general_callbacks.emplace_back([](int status, int note, int velocity) {
            std::cout << "Status: "<< status << " Note: " << note << " Velocity: " << velocity << std::endl;
        });
    }

    AudioBackend::~AudioBackend() {
        ma_device_uninit(&device);
    }

    void AudioBackend::change_generator(int gen_idx) {
        if (gen_idx < 0 || gen_idx >= static_cast<int>(generators.size())) {
            return;
        }
        this->selected_generator = gen_idx;
    }
} // audio