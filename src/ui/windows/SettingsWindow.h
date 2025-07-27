#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H
#include <iostream>

#include "audio/AudioBackend.h"
extern "C" {
#include <microui.h>
}
#include "ui/ui_macros.h"
#include "ui/Window.h"

namespace ui::Windows {
class SettingsWindow final : public ui::Window {
public:
    SettingsWindow(audio::AudioBackend* backend) : ui::Window("Settings", mu_rect(10,10, 300, 400)), backend(backend) {}

protected:
    void OnRender(mu_Context *ctx) override;

private:
    audio::AudioBackend* backend;
};
}

#endif //SETTINGSWINDOW_H
