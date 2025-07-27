#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <iostream>
#include "ui/windowing/RenderWindow.h"

#include <glad/glad.h>

#include "audio/AudioBackend.h"
#include "ui/Renderer.h"
#include "ui/ui_macros.h"
#include "ui/windows/GeneratorManagerWindow.h"
#include "ui/windows/GeneratorsWindow.h"
#include "ui/windows/PianoRollWindow.h"
#include "ui/windows/SettingsWindow.h"

int main() {
    audio::AudioBackend backend;

    ui::RenderWindow window("Evil Studio...");
    ui::Renderer renderer(&window);

    ui::Windows::SettingsWindow settings(&backend);
    ui::Windows::GeneratorsWindow generators(&backend);
    ui::Windows::GeneratorManagerWindow generator_manager(&backend,&generators);
    ui::Windows::PianoRollWindow piano_roll(&backend, &generators);

    window.AddCallback([&] {
        renderer.Begin();
        mu_Context *ctx = renderer.context;

        settings.Render(ctx);
        generators.Render(ctx);
        generator_manager.Render(ctx);
        piano_roll.Render(ctx);

        renderer.Render();
    });

    window.Run();

    return 0;
}
