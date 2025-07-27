#include "GeneratorManagerWindow.h"

#include "audio/Generators/WaveformGenerator.h"
#include <fmt/format.h>

namespace ui {
namespace Windows {

void GeneratorManagerWindow::OnRender(mu_Context *ctx) {
    int cw[] = {-1};
    mu_layout_row(ctx, 1, cw, 0);
    if (generators_window->selected_generator == -1) {
        mu_label(ctx, "No generator selected.");
        return;
    }

    audio::AudioGenerator* gen = backend->generators[generators_window->selected_generator];

    mu_label(ctx, quick_format("Voices {}", gen->voices.size()));

    mu_slider_ex(ctx, &gen->volume, 0.0f, 1.0f, 0.01f, "Volume %.2f", 0);
    mu_slider_ex(ctx, &gen->pan, -1.0f, 1.0f, 0.01f, "Pan %.2f", 0);

    if (auto waveformGen = dynamic_cast<audio::Generators::WaveformGenerator*>(gen)) {
        // int cw_freq[] = { - mu_get_current_container(ctx)->body.w / 2 - ctx->style->padding * 2, -1 };
        // mu_layout_row(ctx, 2, cw_freq, 0);
        mu_layout_row(ctx, 1, cw, 0);

        mu_const_popup_selector(ctx, "Change Waveform", "Waveform", audio::waveform::all_waveforms, audio::waveform::to_string, waveformGen->waveform);
        mu_slider_ex(ctx, &waveformGen->attack, 0.01f, 10.0f, 0.01f, "Attack %.2f s", 0);
        mu_slider_ex(ctx, &waveformGen->decay, 0.01f, 10.0f, 0.01f, "Decay %.2f s", 0);
        mu_slider_ex(ctx, &waveformGen->sustain, 0.0f, 1.0f, 0.01f, "Sustain %.2f", 0);
        mu_slider_ex(ctx, &waveformGen->release, 0.01f, 10.0f, 0.01f, "Release %.2f s", 0);

        float unison = static_cast<float>(waveformGen->unison);
        mu_slider_ex(ctx, &unison, 1.0f, 16.0f, 1.0f, "Unison %.0f", 0);
        waveformGen->unison = static_cast<int>(lroundf(unison));
        mu_slider_ex(ctx, &waveformGen->phase_randomization, 0.0f, 2.0f * M_PI, 0.01f, "Phase Randomization %.2f rad", 0);
    }
}
} // Windows
} // ui