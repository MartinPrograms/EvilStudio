#ifndef WINDOW_H
#define WINDOW_H
#include <functional>
#include <string>
#include <vector>
#include <SDL3/SDL.h>

extern "C" {
#include <microui.h>
}
#include "ui/text/Font.h"

namespace ui {
class RenderWindow {
public:
    explicit RenderWindow(const std::string& title);
    void Run();

    void AddCallback(const std::function<void()>& callback);
    void AddInputCallback(const std::function<void(SDL_Event)>& callback) {
        this->inputCallbacks.push_back(callback);
    }

    int MeasureTextWidth(mu_Font font, const char * str, int len);
    int MeasureTextHeight(mu_Font font);

    int width;
    int height;
private:
    std::string title;
    SDL_Window* window;


    std::vector<std::function<void()>> callbacks;
    std::vector<std::function<void(SDL_Event)>> inputCallbacks;
};
}


#endif //WINDOW_H
