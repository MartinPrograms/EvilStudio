#ifndef MIDIMANAGER_H
#define MIDIMANAGER_H

#include <vector>
#include <functional>

#include <RtMidi.h>

namespace audio {
namespace midi {

struct MidiDevice {
    int port;
    const char *name;
    RtMidiIn *midiin = nullptr;
};

class MidiManager {
public:
    explicit MidiManager();

    std::vector<std::function<void(int, int)>> note_callbacks;
    std::vector<std::function<void(int, int, int)>> general_callbacks;

    [[nodiscard]] bool IsEnabled () const { return enabled; }

private:
    bool enabled = false;
    RtMidiIn  *midiin;
    std::vector<MidiDevice> input_devices;
};

} // midi
} // audio

#endif //MIDIMANAGER_H
