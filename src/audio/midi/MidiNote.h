#ifndef MIDINOTE_H
#define MIDINOTE_H

#include <cstdint>

namespace audio::midi {

    struct MidiNote {
        int note;       // MIDI note number
        int velocity;   // Velocity of the note (0-127)
        int channel;    // MIDI channel (0-15)

        // Not according to the midi spec but data for me!
        float pan;
    };

} // namespace audio::midi

#endif //MIDINOTE_H
