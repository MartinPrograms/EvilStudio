#ifndef FONT_H
#define FONT_H

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <ft2build.h>

#include "ui/Shader.h"

#include FT_FREETYPE_H

namespace ui::text {

struct Character {
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLuint Advance;
};

class Font {
public:
    Font();
    ~Font();

    bool Load(const std::string& filepath, unsigned int pixelSize);
    void Draw(const std::string& text, float x, float y, float scale, const glm::vec3& color);
    [[nodiscard]] glm::vec2 Measure(const std::string& text, float scale) const;

private:
    std::map<char, Character> Characters;
    GLuint VAO, VBO, EBO;
    FT_Library ft;
    FT_Face face;
    bool initialized;
    Shader shader;
    FT_Pos lineHeight;

    void initRenderData();
};
}

#endif //FONT_H
