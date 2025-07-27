#include "MidiManager.h"

#include <cstdlib>
#include <iostream>

namespace audio {
namespace midi {
    void midiInputCallback(double time_stamp, std::vector<unsigned char> * message, void * user_data) {
        MidiManager * manager = static_cast<MidiManager *>(user_data);
        if (manager == nullptr || message->size() < 3) {
            return; // Invalid message
        }
        int status = (*message)[0];
        int note = (*message)[1];
        int velocity = (*message)[2];
        if (status < 0x80 || status > 0xEF) {
            return; // Invalid MIDI status byte
        }

        // If it is a Note On message
        if (status >= 0x90 && status <= 0x9F) {
            for (const auto &callback : manager->note_callbacks) {
                callback(note, velocity);
            }
        }else {
            // Anything else goes to general callbacks
            for (const auto &callback : manager->general_callbacks) {
                callback(status, note, velocity);
            }
        }
    }

MidiManager::MidiManager() {
    enabled = false;

    midiin = new RtMidiIn();

    unsigned int nPorts = midiin->getPortCount();
    if (nPorts == 0) {
        std::cerr << "No MIDI input ports available!" << std::endl;
        return;
    }
    for (unsigned int i = 0; i < nPorts; ++i) {
        std::string portName = midiin->getPortName(i);
        if (portName.empty()) {
            continue;
        }

        MidiDevice device{};
        device.port = i;
        device.name = portName.c_str();
        device.midiin = new RtMidiIn();
        device.midiin->openPort(i);
        device.midiin->setCallback(midiInputCallback, this);
        input_devices.push_back(device);

        std::cout << "Found MIDI input device: " << portName << " (Port " << i << ")" << std::endl;
    }

    // We will not handle output devices for now
    if (input_devices.empty()) {
        enabled = false;
        return;
    }

    enabled = true;
}
} // midi
} // audio