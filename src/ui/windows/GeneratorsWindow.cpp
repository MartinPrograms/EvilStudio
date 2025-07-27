#include "GeneratorsWindow.h"

#include <iostream>
#include <fmt/format.h>

namespace ui {
namespace Windows {

void GeneratorsWindow::OnRender(mu_Context *ctx) {
    int cw[] = {-180, -80,-1};
    mu_layout_row(ctx, 3, cw, 0);

    int idx = 0;
    for (auto& gen : backend->generators) {
        char is_selected = '-';
        if (idx == this->selected_generator) {
            is_selected = 'X';
        }

        mu_label(ctx, quick_format("[{}] {}",
                                is_selected,
                                gen->name));

        mu_slider_ex(ctx, &gen->volume, 0.0f, 1.0f, 0.01f, "vol %.2f", 0);

        mu_push_id(ctx, &idx, sizeof(int));
        if (mu_button(ctx, "Select")) {
            this->selected_generator = idx;
            backend->change_generator(idx);
        }
        mu_pop_id(ctx);

        idx++;
    }
}
} // Windows
} // ui