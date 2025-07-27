#include "Renderer.h"

#include <cstdlib>
#include <stdexcept>

#include <stb_image.h>

#include "ui_macros.h"

ui::RenderWindow* ui::Renderer::current_ctx_window = nullptr;

static int text_width(mu_Font font, const char *text, int len) {
    if (len == -1) { len = static_cast<int>(strlen(text)); }
    int width = ui::Renderer::current_ctx_window->MeasureTextWidth(font, text, len);
    return width;
}

static int text_height(mu_Font font) {
    return ui::Renderer::current_ctx_window->MeasureTextHeight(font);
}

static const char* init_key_map() {
    static char map[256] = { 0 };
    map[SDLK_LSHIFT       & 0xff] = MU_KEY_SHIFT;
    map[SDLK_RSHIFT       & 0xff] = MU_KEY_SHIFT;
    map[SDLK_LCTRL        & 0xff] = MU_KEY_CTRL;
    map[SDLK_RCTRL        & 0xff] = MU_KEY_CTRL;
    map[SDLK_LALT         & 0xff] = MU_KEY_ALT;
    map[SDLK_RALT         & 0xff] = MU_KEY_ALT;
    map[SDLK_RETURN       & 0xff] = MU_KEY_RETURN;
    map[SDLK_BACKSPACE    & 0xff] = MU_KEY_BACKSPACE;
    return map;
}

static const char* key_map = init_key_map();

static const char* init_button_map() {
    static char map[256] = { 0 };
    map[SDL_BUTTON_LEFT   & 0xff] = MU_MOUSE_LEFT;
    map[SDL_BUTTON_RIGHT  & 0xff] = MU_MOUSE_RIGHT;
    map[SDL_BUTTON_MIDDLE & 0xff] = MU_MOUSE_MIDDLE;
    return map;
}

static const char* button_map = init_button_map();

namespace ui {
    Renderer::Renderer(RenderWindow* window) : window(window) {
        this->context = static_cast<mu_Context *>(malloc(sizeof(mu_Context)));
        if (this->context == nullptr) {
            throw std::runtime_error("Failed to allocate memory for mu_Context");
        }

        mu_init(this->context);
        this->context->text_width = text_width;
        this->context->text_height = text_height;

        // A white theme:
        mu_Style* style = this->context->style;
        // style->colors[MU_COLOR_TEXT] = mu_Color(0,0,0,255);
        // style->colors[MU_COLOR_BORDER] = mu_Color(52,52,52,142);
        // style->colors[MU_COLOR_WINDOWBG] = mu_Color(178, 182, 182, 255);
        // style->colors[MU_COLOR_TITLEBG] = mu_Color(145, 149, 149, 255);
        // style->colors[MU_COLOR_TITLETEXT] = mu_Color(240, 240, 240, 255);
        // style->colors[MU_COLOR_PANELBG] = mu_Color(168, 165, 165, 255);
        // style->colors[MU_COLOR_BUTTON] = mu_Color(178, 178, 178, 255);
        // style->colors[MU_COLOR_BUTTONHOVER] = mu_Color(162, 162, 162, 255);
        // style->colors[MU_COLOR_BUTTONFOCUS] = mu_Color(132, 132, 132, 255);
        // style->colors[MU_COLOR_BASE] = mu_Color(165, 165, 165, 255);
        // style->colors[MU_COLOR_BASEHOVER] = mu_Color(125, 125, 125, 255);
        // style->colors[MU_COLOR_BASEFOCUS] = mu_Color(105, 105, 105, 255);
        // style->colors[MU_COLOR_SCROLLBASE] = mu_Color(115, 119, 122, 255);
        // style->colors[MU_COLOR_SCROLLTHUMB] = mu_Color(95, 99, 102, 255);

        // Create a new OpenGL mesh for rendering rectangles
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);

        glBindVertexArray(this->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

        // 4 vertices for a rectangle
        float vertices[] = {
            // Position         // Texture Coords
            0.0f, 0.0f, 0.0f, 0.0f, // Bottom Left
            1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
            1.0f, 1.0f, 1.0f, 1.0f, // Top Right
            0.0f, 1.0f, 0.0f, 1.0f  // Top Left
        };

        uint indices[] = {
            0, 1, 2, // First Triangle
            0, 2, 3  // Second Triangle
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        std::string vertexShaderSource = R"(
            #version 330 core
            layout(location = 0) in vec4 aPos;
            uniform mat4 model;
            uniform mat4 projection;
            uniform mat4 view;
            out vec2 TexCoords;
            void main() {
                gl_Position = projection * view * model * vec4(aPos.xy, 0.0, 1.0);
                TexCoords = aPos.zw;
            }
        )";

        std::string fragmentShaderSource = R"(
            #version 330 core
            in vec2 TexCoords;
            out vec4 color;
            uniform sampler2D rectTexture;
            uniform vec4 rectColor;
            void main() {
                color = rectColor;

}
        )";

        rectShader = Shader();
        if (!rectShader.Load(vertexShaderSource, fragmentShaderSource)) {
            throw std::runtime_error("Failed to load rectangle shader");
        }

        // Create a new font
        auto font = new text::Font();
        if (!font->Load("./resources/font.otf", 18)) {
            throw std::runtime_error("Failed to load font");
        }

        this->context->style->font = reinterpret_cast<mu_Font>(font);

        // Load texture from ./resources/bar.png
        glGenTextures(1, &rectImage);
        glBindTexture(GL_TEXTURE_2D, rectImage);
        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        unsigned char* data = stbi_load("./resources/bar.png", &width, &height, &channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            std::cerr << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture

        mu_Context *ctx = this->context;
        window->AddInputCallback([ctx](SDL_Event e) {
// use the mu_input_* functions

            switch (e.type) {
                case SDL_EVENT_MOUSE_MOTION: mu_input_mousemove(ctx, e.motion.x, e.motion.y); break;
                case SDL_EVENT_MOUSE_WHEEL: mu_input_scroll(ctx, 0, e.wheel.y * -30); break;
                case SDL_EVENT_TEXT_INPUT: mu_input_text(ctx, e.text.text); break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                  int b = button_map[e.button.button & 0xff];
                  if (b && e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) { mu_input_mousedown(ctx, e.button.x, e.button.y, b); }
                  if (b && e.type ==   SDL_EVENT_MOUSE_BUTTON_UP) { mu_input_mouseup(ctx, e.button.x, e.button.y, b);   }
                  break;
                }

                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP: {
                    int c = key_map[e.key.raw & 0xff];
                    if (c && e.type == SDL_EVENT_KEY_DOWN) {
                        mu_input_keydown(ctx, c);
                    }
                    if (c && e.type == SDL_EVENT_KEY_UP) {
                        mu_input_keyup(ctx, c);
                    }
                    break;
                }
            }
        });
    }

    Renderer::~Renderer() {
        current_ctx_window = nullptr;
    }

    /// @brief Begin rendering context! Should be called before any rendering operations.
    void Renderer::Begin() {
        current_ctx_window = this->window;
        mu_begin(this->context);
    }

    void Renderer::DrawText(const char* str, mu_Vec2 pos, mu_Color color, mu_Font font) {
        ((text::Font*)font)->Draw(std::string(str), pos.x, pos.y, 1.0f, glm::vec3(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f));
    }

    void Renderer::DrawRect(glm::vec4 rect, mu_Color color, float rotation) {
        glBindVertexArray(this->VAO);
        rectShader.Use();
        rectShader.SetVec4("rectColor", glm::vec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rectImage);
        rectShader.SetInt("rectTexture", 0);

        rectShader.SetMat4("projection", glm::ortho(
            0.0f, static_cast<float>(Renderer::current_ctx_window->width),
            static_cast<float>(Renderer::current_ctx_window->height), 0.0f));
        rectShader.SetMat4("view", glm::mat4(1.0f)); // Identity view matrix

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(rect.x + rect.z * 0.5f, rect.y + rect.w * 0.5f, 0.0f)); // Move to center
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));               // Apply rotation
        model = glm::translate(model, glm::vec3(-rect.z * 0.5f, -rect.w * 0.5f, 0.0f));                 // Move back
        model = glm::scale(model, glm::vec3(rect.z, rect.w, 1.0f));                                     // Apply scaling

        rectShader.SetMat4("model", model);

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Renderer::DrawIcon(int id, mu_Rect rect, mu_Color color) {
        switch (id) {
            case MU_ICON_CLOSE: {
                glm::vec2 center = glm::vec2(rect.w / 2, rect.h / 2);
                int height = rect.h;
                const float width = 2;

                auto line = glm::vec4(center.x + rect.x - 0.5f, rect.y - 0.5f, width, height);
                DrawRect(line, mu_color(190,40,35,255), 45.0f);
                DrawRect(line, mu_color(190,40,35,255), -45.0f);

                break;
            }
            case MU_ICON_CHECK: {
                glm::vec2 center = glm::vec2(rect.w / 2, rect.h / 2);
                int height = rect.h;
                const float width = 2;

                auto line = glm::vec4(center.x + rect.x - 0.5f, rect.y, width, height);
                DrawRect(line, mu_color(200,220,200,255), 45.0f);
                DrawRect(line, mu_color(200,220,200,255), -45.0f);

                break;
            }
        }
    }

    void Renderer::SetClipRect(mu_Rect rect) {
        int fb_height = Renderer::current_ctx_window->height;

        GLint scissor_x = static_cast<GLint>(rect.x);
        GLint scissor_y = static_cast<GLint>(fb_height - rect.y - rect.h);
        GLsizei scissor_w = static_cast<GLsizei>(rect.w);
        GLsizei scissor_h = static_cast<GLsizei>(rect.h);

        glScissor(scissor_x, scissor_y, scissor_w, scissor_h);
    }

    void Renderer::Render() {
        mu_end(this->context);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glScissor(0, 0, Renderer::current_ctx_window->width, Renderer::current_ctx_window->height);

        mu_Command *cmd = NULL;
        while (mu_next_command(this->context, &cmd)) {
            switch (cmd->type) {
                case MU_COMMAND_TEXT: DrawText(cmd->text.str, cmd->text.pos, cmd->text.color, cmd->text.font); break;
                case MU_COMMAND_RECT: DrawRect(glm::vec4(cmd->rect.rect.x, cmd->rect.rect.y, cmd->rect.rect.w, cmd->rect.rect.h), cmd->rect.color); break;
                case MU_COMMAND_ICON: DrawIcon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
                case MU_COMMAND_CLIP: SetClipRect(cmd->clip.rect); break;
            }
        }

        CheckError("Renderer::Render");
    }
} // ui