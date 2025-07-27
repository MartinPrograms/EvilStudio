#ifndef UI_MACROS_H
#define UI_MACROS_H
#include <functional>
#include <vector>
#include <string_view>
#include <fmt/format.h>

extern "C"{
#include <microui.h>
}

static mu_Rect intersect_rects(mu_Rect r1, mu_Rect r2) {
    int x1 = mu_max(r1.x, r2.x);
    int y1 = mu_max(r1.y, r2.y);
    int x2 = mu_min(r1.x + r1.w, r2.x + r2.w);
    int y2 = mu_min(r1.y + r1.h, r2.y + r2.h);
    if (x2 < x1) { x2 = x1; }
    if (y2 < y1) { y2 = y1; }
    return mu_rect(x1, y1, x2 - x1, y2 - y1);
}

#define UI_WINDOW(ctx, name, x,y,w,h) \
    for (bool _open = mu_begin_window(ctx, name, mu_rect(x,y,w,h)); \
    _open; \
    _open = false, mu_end_window(ctx))

#define UI_LAYOUT_VAR(ctx, var, count)                                    \
    int per_item = (mu_get_current_container(ctx)->body.w / (count))      \
        - 2 * ctx->style->padding;                                        \
    int var[count];                                                       \
    for (int & _ui_i : cw) _ui_i = per_item;

#define UI_LAYOUT_WIDTH(ctx) \
    mu_get_current_container(ctx)->body.w - ctx->style->padding * 2

#define UI_BUTTON(ctx, lbl) mu_button(ctx, lbl)
#define UI_CHECK(ctx, lbl, b) mu_checkbox(ctx, lbl, &b)

#define UI_SLIDER(ctx, lbl, fptr, flow, fhigh, fstep) \
    mu_label(ctx, lbl); \
    mu_slider_ex(ctx, fptr, flow, fhigh, fstep, "%.2f", 0)

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

inline void UI_SEPARATOR(mu_Context* ctx) {
    mu_Rect __temp_rect = mu_layout_next(ctx); \
    __temp_rect.h = ctx->style->padding; \
    __temp_rect.y += ctx->style->padding * 2; \
    mu_draw_rect(ctx, __temp_rect, ctx->style->colors[MU_COLOR_BORDER]);
}

template <typename... Args>
inline const char* quick_format(fmt::format_string<Args...> fmt_str, Args&&... args) {
    thread_local std::vector<char> buf;
    buf.clear();
    fmt::format_to(std::back_inserter(buf), fmt_str, std::forward<Args>(args)...);
    buf.push_back('\0');
    return buf.data();
}

template <class Container, class T, class ToString = std::function<const char*(const T&)>>
void mu_const_popup_selector(mu_Context* ctx,
                       const char* popup_id,
                       const std::string& label,
                       const Container& options,
                       ToString to_string,
                       T& current) {
    if (mu_button(ctx, quick_format("{}: {}", label, to_string(current)))) {
        mu_open_popup(ctx, popup_id);
    }
    if (mu_begin_popup(ctx, popup_id)) {
        for (const auto& option : options) {
            if (mu_button(ctx, to_string(option))) {
                current = option;
            }
        }
        mu_end_popup(ctx);
    }
}

// mu_popup_selector (based on lambdas instead)
// General version with separate conversion functions
template <class Container, class T,
          class ToStringOption = std::function<const char*(const typename Container::value_type&)>,
          class ToStringCurrent = std::function<const char*(const T&)>,
          class GetValue = std::function<T(const typename Container::value_type&)>>
void mu_popup_selector(mu_Context* ctx,
                       const char* popup_id,
                       const std::string& label,
                       const Container& options,
                       ToStringOption to_string_option,
                       ToStringCurrent to_string_current,
                       GetValue get_value,
                       T& current) {
    if (mu_button(ctx, quick_format("{}: {}", label, to_string_current(current)))) {
        mu_open_popup(ctx, popup_id);
    }
    if (mu_begin_popup(ctx, popup_id)) {
        int cw[] = {0};// default size.
        for (const auto& option: options) {
            // Measure the width of the option string
            int width = ctx->text_width(ctx->style->font, to_string_option(option), strlen(to_string_option(option)));
            if (width > cw[0]) {
                cw[0] = width + ctx->style->padding * 2; // Add padding for better appearance
            }
        }
        mu_layout_row(ctx, 1, cw, 0); // Use the calculated width for the popup layout
        for (const auto& option : options) {
            if (mu_button(ctx, to_string_option(option))) {
                current = get_value(option);
            }
        }
        mu_end_popup(ctx);
    }
}

// Overload for when T matches container type
template <class Container, class T, class ToString = std::function<const char*(const T&)>>
void mu_popup_selector(mu_Context* ctx,
                       const char* popup_id,
                       const std::string& label,
                       const Container& options,
                       ToString to_string,
                       T& current) {
    mu_popup_selector(
        ctx, popup_id, label, options,
        to_string,
        to_string,
        [](const T& x) -> T { return x; },
        current
    );
}

inline void mu_easy_popup(mu_Context * ctx, const char* popup_id, const char* label) {
    if (mu_begin_popup(ctx, popup_id)) {
        int cw[] = {0}; // default size.
        int width = ctx->text_width(ctx->style->font, label, strlen(label));
        cw[0] = width + ctx->style->padding * 2; // Add padding for better appearance
        mu_layout_row(ctx, 1, cw, 0); // Use the calculated width for the popup layout
        mu_label(ctx, label);
        mu_end_popup(ctx);
    }
}

#endif //UI_MACROS_H
