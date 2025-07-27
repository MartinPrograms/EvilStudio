#include "Font.h"

#include <iostream>

#include "ui/Renderer.h"

namespace ui::text {

Font::Font() : VAO(0), VBO(0), ft(nullptr), face(nullptr), initialized(false){
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    // Create the shader
    std::string vertSource = R"(
        #version 330 core
        layout(location = 0) in vec4 vertex; // x, y, z, w
        uniform mat4 model; // Model matrix for transformations
        uniform mat4 projection; // Projection matrix
        uniform mat4 view; // View matrix
        out vec2 TexCoords;
        void main() {
            gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex.zw;
        }
    )";

    std::string fragSource = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 color;
        uniform sampler2D text;
        uniform vec3 textColor;
        void main() {
            vec2 texCoords = TexCoords;
            float alpha = texture(text, texCoords).r; // Get the alpha value from the texture
            if (alpha < 0.1) {
                discard; // Discard pixels with low alpha
            }
            color = vec4(textColor, alpha);
        }
    )";

    shader = Shader();
    if (!shader.Load(vertSource, fragSource)) {
        initialized = false;
        std::cerr << "ERROR::SHADER: Failed to load font shader" << std::endl;
    }
}

Font::~Font() {
    if (face) FT_Done_Face(face);
    if (ft) FT_Done_FreeType(ft);
    if (initialized) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        for (auto& kv : Characters) {
            glDeleteTextures(1, &kv.second.TextureID);
        }
    }
}

bool Font::Load(const std::string &filepath, unsigned int pixelSize) {
    if (!ft) return false;
    // Load font face
    if (FT_New_Face(ft, filepath.c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font: " << filepath << std::endl;
        return false;
    }
    FT_Set_Pixel_Sizes(face, 0, pixelSize);
    this->lineHeight = (face->size->metrics.height >> 6);
    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; ++c) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYTPE: Failed to load Glyph for char " << c << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            static_cast<int>(face->glyph->bitmap.width),
            static_cast<int>(face->glyph->bitmap.rows),
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store character
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width,  face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left,   face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };


        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    Renderer::CheckError("Font::Load");

    initRenderData();
    initialized = true;
    return true;
}

void Font::initRenderData() {
    shader.Use();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

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

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    // vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    // Create EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Renderer::CheckError("Font::initRenderData");
}

void Font::Draw(const std::string &text, float x, float y, float scale, const glm::vec3 &color) {
    if (!initialized) return;

    shader.Use(); // Use shader once
    shader.SetVec3("textColor", color);
    shader.SetInt("text", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (char c : text) {
        Character ch = Characters[c];

        float xpos = x + ch.Bearing.x * scale;
        // y is the baseline (distance from top of screen),
        // bearing.y is how far *above* the baseline the glyph extends.
        float ypos = y - ch.Bearing.y * scale;
        ypos += this->Measure("A", scale).y; // Adjust for height of the font

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Calculate the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
        model = glm::scale(model, glm::vec3(w, h, 1.0f));
        shader.SetMat4("model", model);
        shader.SetMat4("projection", glm::ortho(0.0f, static_cast<float>(Renderer::current_ctx_window->width), static_cast<float>(Renderer::current_ctx_window->height), 0.0f));
        shader.SetMat4("view", glm::mat4(1.0f)); // Identity view matrix

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        Renderer::CheckError("Font::Draw");

        x += static_cast<float>(ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

glm::vec2 Font::Measure(const std::string &text, float scale) const {
    float width = 0.0f;
    float height = 0.0f;
    for (char c : text) {
        if (!Characters.contains(c))
            continue; // the show must go on...
        const Character& ch = Characters.at(c);
        width += (ch.Advance >> 6) * scale; // Advance is in 1/64th pixels
        if (ch.Size.y == 0) continue; // Skip characters with no height
        float h = static_cast<float>(ch.Size.y) * scale;
        if (h > height) height = h;
    }

    return {width, height};
}


}
