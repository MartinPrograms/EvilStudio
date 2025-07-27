#include "RenderWindow.h"

#include <iostream>
#include <glad/glad.h>

#include "SDL3/SDL_opengl_glext.h"

ui::RenderWindow::RenderWindow(const std::string &title) : width(1920), height(1080), title(title) {
    // Create the SDL window
    this->window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    SDL_ShowWindow(this->window);

    SDL_GL_CreateContext(window);

    if (gladLoadGL() == 0) {
        std::printf("Failed to initialize OpenGL context.\n");
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_TEXTURE_2D);
}

void ui::RenderWindow::Run() {
    bool shouldRun = true;
    while (shouldRun) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                shouldRun = false;
            }

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                this->width = event.window.data1;
                this->height = event.window.data2;
            }

            for (const auto& inputCallback : this->inputCallbacks) {
                inputCallback(event);
            }
        }

        glViewport(0, 0, this->width, this->height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto& callback : this->callbacks) {
            callback();
        }

        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ui::RenderWindow::AddCallback(const std::function<void()>& callback) {
    this->callbacks.push_back(callback);
}

int ui::RenderWindow::MeasureTextWidth(mu_Font font, const char *str, int len) {
    return ((text::Font*)font)->Measure(std::string(str, len), 1.0f).x;
}

int ui::RenderWindow::MeasureTextHeight(mu_Font font) {
    return ((text::Font*)font)->Measure("A", 1.0f).y; // Measure height using a single character
    // Bit ugly but whatever.
}

