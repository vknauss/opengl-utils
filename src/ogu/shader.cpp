#include "shader.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#define SHADER_PROGRAM_ERR_NO_ACTIVE_UNIFORM 0


namespace ogu {

Shader::Shader(const std::string& filename, Shader::Type type) {
    std::ifstream fs(filename);
    if (!fs) {
        throw std::runtime_error("Failed to load shader file: \"" + filename + "\".");
    }
    std::string codeStr {std::istreambuf_iterator<char>(fs),
                         std::istreambuf_iterator<char>()};

    if (!(handle = glCreateShader(type))) {
        throw std::runtime_error("Failed to create shader object.");
    }

    const GLchar* code = codeStr.c_str();
    glShaderSource(handle, 1, &code, nullptr);
    glCompileShader(handle);

    GLint status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint infoLogLength;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> infoLog(infoLogLength);
        glGetShaderInfoLog(handle, infoLogLength, nullptr, infoLog.data());
        std::cerr << infoLog.data() << std::endl;
        throw std::runtime_error("Shader compile failed. Source file: " + filename);
    }
}

Shader::~Shader() {
    glDeleteShader(handle);
}

ShaderProgram::ShaderProgram(const std::initializer_list<Shader>& shaders) {
    handle = glCreateProgram();
    for (const auto& shader : shaders) {
        glAttachShader(handle, shader.handle);
    }

    glLinkProgram(handle);

    for (const auto& shader : shaders) {
        glDetachShader(handle, shader.handle);
    }

    GLint status;
    glGetProgramiv(handle, GL_LINK_STATUS, &status);
    if (!status) {
        GLint infoLogLength;
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> infoLog(infoLogLength);
        glGetProgramInfoLog(handle, infoLogLength, nullptr, infoLog.data());
        std::cerr << infoLog.data() << std::endl;
        throw std::runtime_error("Shader program link failed.");
    }

    
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(handle);
}

void ShaderProgram::addUniform(const std::string& name) {
    GLint location = glGetUniformLocation(handle, name.c_str());
    #if SHADER_PROGRAM_ERR_NO_ACTIVE_UNIFORM == 1
    if (location == -1) throw std::runtime_error("Uniform name \"" + name + "\" is not an active uniform in the program.");
    #endif
    uniformLocations[name] = location;
}

void ShaderProgram::addUniformBuffer(const std::string& name) {
    GLint index = glGetUniformBlockIndex(handle, name.c_str());
    #if SHADER_PROGRAM_ERR_NO_ACTIVE_UNIFORM == 1
    if (index == -1) throw std::runtime_error("Uniform block name \"" + name + "\" is not an active uniform block in the program.");
    #endif
    uniformBufferIndices[name] = index;
}

// Scalar types: int, unsigned int, float

template<>
void ShaderProgram::setUniform(const std::string& name, int value) const {
    glUniform1i(uniformLocations.at(name), value);
}

template<>
void ShaderProgram::setUniform(const std::string& name, unsigned int value) const {
    glUniform1ui(uniformLocations.at(name), value);
}

template<>
void ShaderProgram::setUniform(const std::string& name, float value) const {
    glUniform1f(uniformLocations.at(name), value);
}

GLint ShaderProgram::getUniformLocation(const std::string& name) const {
    return uniformLocations.at(name);
}

void ShaderProgram::bindUniformBuffer(const std::string& name, uint32_t binding, const buffer& buffer, intptr_t offset, size_t size) const {
    glUniformBlockBinding(handle, uniformBufferIndices.at(name), binding);
    glBindBufferRange(GL_UNIFORM_BUFFER, binding, buffer.handle(), offset, size);
}

}  // namespace ogu
