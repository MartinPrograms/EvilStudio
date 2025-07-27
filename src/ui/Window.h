#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <string>
extern "C" {
#include "microui.h"
}
namespace ui {

class Window {
public:
    Window(const std::string& title, const mu_Rect& rect)
        : title_(title), rect_(rect), open_(true) {}
    virtual ~Window() = default;

    void Render(mu_Context* ctx) {
        if (open_ && mu_begin_window(ctx, title_.c_str(), rect_)) {
            OnRender(ctx);
            mu_end_window(ctx);
        }
    }

    void SetOpen(bool open) { open_ = open; }
    [[nodiscard]] bool IsOpen() const       { return open_; }

protected:
    virtual void OnRender(mu_Context* ctx) = 0;

    const std::string title_;
    mu_Rect rect_;

private:
    bool open_;
};

} // namespace ui

#endif // UI_WINDOW_H
