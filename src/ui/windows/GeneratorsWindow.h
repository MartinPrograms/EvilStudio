#ifndef GENERATORSWINDOW_H
#define GENERATORSWINDOW_H
#include "audio/AudioBackend.h"
#include "ui/Window.h"

extern "C" {
#include <microui.h>
}

#include "ui/ui_macros.h"

namespace ui {
namespace Windows {

class GeneratorsWindow final : public ui::Window {
public:
    GeneratorsWindow(audio::AudioBackend* backend) : ui::Window("Generators", mu_rect(10, 410, 500, 300)),
                                                     selected_generator(-1), backend(backend) {
    }

    int selected_generator;
protected:
    void OnRender(mu_Context *ctx) override;
private:
    audio::AudioBackend* backend;
};

} // Windows
} // ui

#endif //GENERATORSWINDOW_H
