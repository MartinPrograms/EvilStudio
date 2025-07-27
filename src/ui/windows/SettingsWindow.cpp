#include "SettingsWindow.h"

void ui::Windows::SettingsWindow::OnRender(mu_Context *ctx) {
    int cw[1] = {UI_LAYOUT_WIDTH(ctx)};
    mu_layout_row(ctx, 1, cw, 0);

    UI_SLIDER(ctx, "Master Volume:", &this->backend->master_volume, 0.0f, 1.0f, 0.01f);
    UI_SLIDER(ctx, "Master Pan:", &this->backend->master_pan, -1.0f, 1.0f, 0.1f);

    UI_SEPARATOR(ctx);

    int width = mu_get_current_container(ctx)->body.w / 3 - ctx->style->padding;
    int pbcw[] = {width, width, -1}; // Play, Pause, Stop buttons
    mu_layout_row(ctx, 3, pbcw, 0);
    if (mu_button(ctx, "Play")) {
        this->backend->sequencer_state.start();
    }
    if (mu_button(ctx, "Pause")) {
        this->backend->sequencer_state.pause();
    }
    if (mu_button(ctx, "Stop")) {
        this->backend->sequencer_state.reset();
    }

    mu_layout_row(ctx, 1, cw, 0);

    float current_time = static_cast<float>(this->backend->sequencer_state.get_current_sample()) / SAMPLE_RATE;
    int minutes = static_cast<int>(current_time / 60);
    int seconds = static_cast<int>(current_time) % 60;
    int milliseconds = static_cast<int>((current_time - static_cast<int>(current_time)) * 1000);
    mu_label(ctx, quick_format("{}:{}:{}",
                               fmt::format("{:02}", minutes),
                               fmt::format("{:02}", seconds),
                               fmt::format("{:03}", milliseconds)));
    mu_label(ctx, quick_format("Current Sample: {}", this->backend->sequencer_state.get_current_sample()));
    mu_label(ctx, quick_format("Slice: {}", this->backend->sequencer_state.get_current_slice()));
    mu_label(ctx, quick_format("Bucket: {}", this->backend->sequencer_state.get_current_bucket()));
}
