#ifndef NOTESEQUENCE_H
#define NOTESEQUENCE_H

#include <vector>

#include "Note.h"
#include "audio/AudioGenerator.h"

namespace audio {
namespace Sequencing {

class NoteSequence {
public:
    std::vector<Note> notes; // List of notes in this sequence
    AudioGenerator *generator = nullptr; // Pointer to the generator this sequence is associated with

    void update(uint64_t current_sample) {
        // Update the sequence based on the current sample
        for (auto& note : notes) {
            if (!note.is_playing && note.play_time <= current_sample && note.stop_time > current_sample) {
                // Note is currently active, process it
                generator->NoteOn(note);
                note.is_playing = true; // Mark the note as playing
            } else if (note.stop_time <= current_sample) {
                // Note has ended, send NoteOff
                generator->NoteOff(note);
            }
        }
    }
};

} // Sequencing
} // audio

#endif //NOTESEQUENCE_H
