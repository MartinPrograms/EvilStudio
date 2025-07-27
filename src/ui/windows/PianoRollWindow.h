#ifndef PIANOROLLWINDOW_H
#define PIANOROLLWINDOW_H
#include "GeneratorsWindow.h"
#include "audio/AudioBackend.h"
#include "ui/Window.h"

namespace ui {
namespace Windows {

class PianoRollWindow final : public ui::Window{
public:
    PianoRollWindow(audio::AudioBackend* backend, Windows::GeneratorsWindow* generators_window) : ui::Window("Piano Roll", mu_rect(610, 10, 800, 600)), generators_window(generators_window), backend(backend) {
    }

protected:
    void OnRender(mu_Context *ctx) override;
private:
    Windows::GeneratorsWindow* generators_window;
    audio::AudioBackend* backend;
    int64_t selected_pattern = -1; // Currently selected pattern in the piano roll

    void RenderPianoRoll(mu_Context *ctx, const audio::Sequencing::NoteSequence &sequence);
};

} // Windows
} // ui

#endif //PIANOROLLWINDOW_H
