#ifndef NOTES_H
#define NOTES_H
#include <array>
#include <cmath>

namespace audio::piano {
    enum class Note {
        A,
        ASharp,
        B,
        C,
        CSharp,
        D,
        DSharp,
        E,
        F,
        FSharp,
        G,
        GSharp
    };

    constexpr std::array<Note, 12> all_notes = {
        Note::A, Note::ASharp, Note::B,Note::C,
        Note::CSharp, Note::D, Note::DSharp,
        Note::E, Note::F, Note::FSharp, Note::G,
        Note::GSharp
    };

    constexpr const char* to_string(Note note) {
        switch (note) {
            case Note::C:       return "C";
            case Note::CSharp:  return "C#";
            case Note::D:       return "D";
            case Note::DSharp:  return "D#";
            case Note::E:       return "E";
            case Note::F:       return "F";
            case Note::FSharp:  return "F#";
            case Note::G:       return "G";
            case Note::GSharp:  return "G#";
            case Note::A:       return "A";
            case Note::ASharp:  return "A#";
            case Note::B:       return "B";
            default:            return "Unknown";
        }
    }

    constexpr float note_frequency(Note note, int octave) {
        // A4 is 440 Hz
        const float base_frequency = 440.0f;
        const int semitones_from_A4 = static_cast<int>(note) - static_cast<int>(Note::A) + (octave - 4) * 12;
        return base_frequency * std::pow(2.0f, semitones_from_A4 / 12.0f);
    }

    constexpr int note_to_midi(Note note, int octave) {
        // MIDI note numbers start at C-1 (MIDI note 0)
        // A4 is MIDI note 69
        return static_cast<int>(note) + (octave + 1) * 12 + 21; // 21 is the offset for MIDI note numbers
    }

    constexpr float midi_to_frequency(int midi_note) {
        // Convert to note and octave, then pass to note_frequency (this later means we can use frequency mapping)
        int octave = (midi_note / 12) - 1; // MIDI note 0 is C-1, so we subtract 1
        int note_index = midi_note % 12; // Get the note index within the octave
        if (note_index < 0 || note_index >= static_cast<int>(all_notes.size())) {
            return 0.0f; // Invalid MIDI note
        }
        return note_frequency(all_notes[note_index], octave);
    }
}

#endif //NOTES_H
