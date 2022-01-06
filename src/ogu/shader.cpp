#include "shader.h"

#include <stdexcept>

#define SHADER_PROGRAM_ERR_NO_ACTIVE_UNIFORM 0


namespace ogu {

shader::shader(const std::vector<std::string>& sources, shader::type type) {
    handle = glCreateShader((GLenum) type);

    std::vector<const GLchar*> codeStrings(sources.size());
    for (auto i = 0u; i < sources.size(); ++i)
        codeStrings[i] = sources[i].c_str();

    glShaderSource(handle, sources.size(), codeStrings.data(), nullptr);
    glCompileShader(handle);

    GLint status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint infoLogLength;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> infoLog(infoLogLength);
        glGetShaderInfoLog(handle, infoLogLength, nullptr, infoLog.data());
        throw std::runtime_error(std::string("Shader compile failed. ") + infoLog.data());
    }
}

shader::~shader() {
    glDeleteShader(handle);
}

shader_program::shader_program(const std::initializer_list<shader>& shaders) {
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
        throw std::runtime_error(std::string("Shader program link failed.") + infoLog.data());
    }

    
}

shader_program::~shader_program() {
    glDeleteProgram(handle);
}

void shader_program::addUniform(const std::string& name) {
    GLint location = glGetUniformLocation(handle, name.c_str());
    #if SHADER_PROGRAM_ERR_NO_ACTIVE_UNIFORM == 1
    if (location == -1) throw std::runtime_error("Uniform name \"" + name + "\" is not an active uniform in the program.");
    #endif
    uniformLocations[name] = location;
}

void shader_program::addUniformBuffer(const std::string& name) {
    auto& i = uniformBufferIndices[name];
    i.index = glGetUniformBlockIndex(handle, name.c_str());
    i.binding = num_ubo_bindings++;
    #if SHADER_PROGRAM_ERR_NO_ACTIVE_UNIFORM == 1
    if (index == -1) throw std::runtime_error("Uniform block name \"" + name + "\" is not an active uniform block in the program.");
    #endif
    glUniformBlockBinding(handle, i.index, i.binding);
}

// Scalar types: int, unsigned int, float

template<>
void shader_program::setUniform(const std::string& name, int value) const {
    glUniform1i(uniformLocations.at(name), value);
}

template<>
void shader_program::setUniform(const std::string& name, unsigned int value) const {
    glUniform1ui(uniformLocations.at(name), value);
}

template<>
void shader_program::setUniform(const std::string& name, float value) const {
    glUniform1f(uniformLocations.at(name), value);
}

GLint shader_program::getUniformLocation(const std::string& name) const {
    return uniformLocations.at(name);
}

void shader_program::bindUniformBuffer(const std::string& name, const buffer& buffer) const {
    const auto& i = uniformBufferIndices.at(name);
    // glUniformBlockBinding(handle, i.index, i.binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, i.binding, buffer.handle());
}

void shader_program::bindUniformBuffer(const std::string& name, const buffer& buffer, intptr_t offset, size_t size) const {
    const auto& i = uniformBufferIndices.at(name);
    // glUniformBlockBinding(handle, i.index, i.binding);
    glBindBufferRange(GL_UNIFORM_BUFFER, i.binding, buffer.handle(), offset, size);
}

}  // namespace ogu
