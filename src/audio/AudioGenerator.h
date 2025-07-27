#ifndef GENERATOR_H
#define GENERATOR_H

#include <algorithm>
#include <array>
#include <atomic>
#include <ranges>
#include <utility>

#include <string>
#include <unordered_map>
#include <vector>

#include "audio_math.h"
#include "piano.h"
#include "Sequencing/Note.h"
#include "Sequencing/Voice.h"

namespace audio {

class AudioGenerator {
public:
    explicit AudioGenerator(std::string name) : name(std::move(name)) {

    }
    virtual ~AudioGenerator() = default;
    float volume = 0.5f;
    float pan = 0.0f;

    std::string name;

    virtual void Process(float *buffer, int channels, int buffer_size, uint64_t current_sample) = 0;

    void NoteOn(Sequencing::Note note) {
        int buffer_index = write_buffer_index.load(std::memory_order_relaxed);
        scheduled_note_buffer_on[buffer_index].emplace_back(note);
    }

    void NoteOff(Sequencing::Note note) {
        int buffer_index = write_buffer_index.load(std::memory_order_relaxed);
        scheduled_note_buffer_off[buffer_index].emplace_back(note); // Velocity is not used for NoteOff
    }

    std::vector<Sequencing::Voice> voices; // Currently playing voices
protected:
    std::vector<Sequencing::Note> scheduled_note_buffer_on[2];
    std::vector<Sequencing::Note> scheduled_note_buffer_off[2];
    std::atomic<int> write_buffer_index = 0;
private:

};

} // audio

#endif //GENERATOR_H
