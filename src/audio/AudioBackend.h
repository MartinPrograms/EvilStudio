#ifndef AUDIOBACKEND_H
#define AUDIOBACKEND_H

#include <memory>
#include <miniaudio.h>
#include <vector>

#include "AudioGenerator.h"
#include "midi/MidiManager.h"
#include "Sequencing/SequencerState.h"

namespace audio {

class AudioBackend {
public:
    static AudioBackend* audio_backend;

    AudioBackend();
    ~AudioBackend();

    float master_volume = 1.0f;
    float master_pan = 0.0f;

    std::pmr::vector<AudioGenerator*> generators;

    Sequencing::SequencerState sequencer_state;

    midi::MidiManager midi_manager;

    void change_generator(int gen_idx);
private:
    int selected_generator = -1;
    ma_device device{};
};

} // audio

#endif //AUDIOBACKEND_H
