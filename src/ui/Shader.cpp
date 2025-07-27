#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace ui {

Shader::Shader()
    : programID(0) {}

Shader::~Shader() {
    if (programID) {
        glDeleteProgram(programID);
    }
}

bool Shader::Load(const std::string& vertexSource,
                  const std::string& fragmentSource,
                  const std::string& geometryPath) {
    // Read shader source code
    std::string vertexCode =(vertexSource);
    std::string fragmentCode =(fragmentSource);
    std::string geometryCode;
    if (!geometryPath.empty()) {
        geometryCode = (geometryPath);
    }

    // Compile shaders
    unsigned int vertexID = CompileShader(vertexCode, GL_VERTEX_SHADER);
    unsigned int fragmentID = CompileShader(fragmentCode, GL_FRAGMENT_SHADER);
    unsigned int geometryID = 0;
    if (!geometryPath.empty()) {
        geometryID = CompileShader(geometryCode, GL_GEOMETRY_SHADER);
    }

    // Link program
    programID = glCreateProgram();
    glAttachShader(programID, vertexID);
    glAttachShader(programID, fragmentID);
    if (geometryID) glAttachShader(programID, geometryID);
    glLinkProgram(programID);

    // Check linking errors
    int success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }

    // Cleanup individual shaders
    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
    if (geometryID) glDeleteShader(geometryID);

    return true;
}

void Shader::Use() const {
    glUseProgram(programID);
}

void Shader::SetBool(const std::string& name, bool value) const {
    glUniform1i(GetUniformLocation(name), static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

unsigned int Shader::Id() const {
    return programID;
}

unsigned int Shader::CompileShader(const std::string& source, unsigned int type) const {
    unsigned int shaderID = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::"
                  << (type == GL_VERTEX_SHADER ? "VERTEX" :
                      type == GL_FRAGMENT_SHADER ? "FRAGMENT" : "GEOMETRY")
                  << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shaderID;
}

int Shader::GetUniformLocation(const std::string& name) const {
    if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
        return uniformLocationCache[name];
    }
    int location = glGetUniformLocation(programID, name.c_str());
    uniformLocationCache[name] = location;
    return location;
}

} // namespace ui
