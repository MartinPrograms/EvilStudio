#ifndef GENERATORMANAGERWINDOW_H
#define GENERATORMANAGERWINDOW_H
#include "GeneratorsWindow.h"
#include "audio/AudioBackend.h"
#include "ui/Window.h"
extern "C"{
#include <microui.h>
}

namespace ui {
namespace Windows {

class GeneratorManagerWindow final : public ui::Window {
public:
    GeneratorManagerWindow(audio::AudioBackend* backend, GeneratorsWindow* generators_window) : ui::Window("Generator Manager", mu_rect(310,10, 260, 400)), generators_window(generators_window), backend(backend) {}

protected:
    void OnRender(mu_Context *ctx) override;


private:
    GeneratorsWindow* generators_window;
    audio::AudioBackend* backend;

};
} // Windows
} // ui

#endif //GENERATORMANAGERWINDOW_H
