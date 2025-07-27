#ifndef SEQUENCERSTATE_H
#define SEQUENCERSTATE_H
#include <cstdint>
#include <functional>
#include <vector>

#include "Pattern.h"
#include "audio/AudioDefinitions.h"

namespace audio {
namespace Sequencing {

class SequencerState {
public:
    SequencerState() = default;

    void move_forward(uint64_t samples) {
        if (is_playing){
            current_sample += samples;
            for (auto& pattern : patterns) {
                pattern.update(current_sample);
            }
        }
    }

    void processed(uint64_t samples) {
        current_process_sample += samples;
    }

    [[nodiscard]] uint64_t get_current_sample() const {
        return current_sample;
    }

    [[nodiscard]] uint64_t get_current_process_sample() const {
        return current_process_sample;
    }

    void reset() {
        current_sample = 0;
        is_playing = false;
        for (auto& callback : reset_callbacks) {
            callback();
        }

        // Reset all patterns
        for (auto& pattern : patterns) {
            pattern.stop(); // Stop all note sequences in the pattern
        }
    }

    void start() {
        is_playing = true;
    }

    void pause() {
        is_playing = false;
        for (auto& pattern : patterns) {
            pattern.pause(); // Stop all note sequences in the pattern
        }
    }

    [[nodiscard]] bool is_playing_state() const {
        return is_playing;
    }

    [[nodiscard]] uint64_t get_current_slice() const {
        return current_sample % SLICE_SIZE;
    }

    [[nodiscard]] uint64_t get_current_bucket() const {
        return current_sample / SLICE_SIZE;
    }

    std::vector<std::function<void()>> reset_callbacks;

    std::vector<Pattern> patterns;
    int id_counter = 0; // Counter for unique pattern IDs

private:
    uint64_t current_sample = 0;
    uint64_t current_process_sample = 0;
    bool is_playing = false;
};

} // Sequencing
} // audio

#endif //SEQUENCERSTATE_H
