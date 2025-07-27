#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace ui {

class Shader {
public:
    Shader();
    ~Shader();

    // Load, compile and link vertex & fragment shaders (optionally geometry shader)
    bool Load(const std::string& vertexSource,
              const std::string& fragmentSource,
              const std::string& geometryPath = "");

    // Activate the shader program
    void Use() const;

    // Uniform setters
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;

    // Get program ID
    unsigned int Id() const;

private:
    unsigned int programID;

    mutable std::unordered_map<std::string, int> uniformLocationCache;

    // Helper methods
    std::string ReadFile(const std::string& filePath) const;
    unsigned int CompileShader(const std::string& source, unsigned int type) const;
    int GetUniformLocation(const std::string& name) const;
};


} // ui

#endif //SHADER_H
