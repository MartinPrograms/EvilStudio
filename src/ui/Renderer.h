#ifndef RENDERER_H
#define RENDERER_H
#include "text/Font.h"
extern "C" {
#include <microui.h>
}

#include "windowing/RenderWindow.h"

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>

namespace ui {

class Renderer {
public:
    explicit Renderer(RenderWindow* window);
    ~Renderer();

    void Begin();
    void Render();

    mu_Context *context;
    static ui::RenderWindow *current_ctx_window;

    static void CheckError(std::string context) {
        GLenum err;
        auto error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL Error in " << context << ": " << error << std::endl;
            exit(EXIT_FAILURE);
        }
    }

private:

    void DrawText(const char *str, mu_Vec2 pos, mu_Color color, mu_Font font);
    void DrawRect(glm::vec4 rect, mu_Color color, float rotation = 0.0f);
    void DrawIcon(int id, mu_Rect rect, mu_Color color);
    void SetClipRect(mu_Rect rect);

    GLuint VAO, VBO, EBO;
    GLuint rectImage;
    Shader rectShader;

    RenderWindow* window;
};

} // ui

#endif //RENDERER_H
