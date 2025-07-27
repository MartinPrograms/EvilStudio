#ifndef NOTE_H
#define NOTE_H

#include <cstdint>

namespace audio::Sequencing {
    struct Note {
        int note_number;
        int velocity; // 0-127

        float pan = 0.0f;

        // Below is to be used by the sequencer:
        uint64_t play_time = 0; // When the note should be played
        uint64_t stop_time = 0; // When the note should be stopped
        bool is_playing = false; // Whether the note is currently playing
    };
}

#endif //NOTE_H
