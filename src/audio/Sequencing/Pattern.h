#ifndef PATTERN_H
#define PATTERN_H
#include "NoteSequence.h"
#include <vector>

namespace audio {
namespace Sequencing {

class Pattern {
    // A pattern holds a list of note sequences associated with a specific generator.
public:
    std::string name = "Pattern";
    int id; // Unique identifier for the pattern
    std::vector<NoteSequence> note_sequences;
    void update(uint64_t current_sample) {
        // Update all note sequences in this pattern
        for (auto& sequence : note_sequences) {
            sequence.update(current_sample);
        }
    }

    void stop() {
        // Stop all note sequences in this pattern
        for (auto& sequence : note_sequences) {
            for (auto& note : sequence.notes) {
                if (note.is_playing) {
                    sequence.generator->NoteOff(note);
                    note.is_playing = false; // Mark the note as not playing
                }
            }
        }
    }

    void pause() {
        // Pause all note sequences in this pattern
        for (auto& sequence : note_sequences) {
            for (auto& note : sequence.notes) {
                if (note.is_playing) {
                    sequence.generator->NoteOff(note);
                }
            }
        }
    }
};

} // Sequencing
} // audio

#endif //PATTERN_H
