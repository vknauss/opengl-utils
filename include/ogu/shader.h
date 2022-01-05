#pragma once

#include <GL/glew.h>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "buffer.h"


namespace ogu {

class shader {

    friend class shader_program;

    GLuint handle;

public:

    enum class type {
        VERTEX = GL_VERTEX_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER
    };

    shader(const std::vector<std::string>& sources, type type);
    shader(shader&&);

    ~shader();

};

class shader_program {
private:

    GLuint handle;

    std::unordered_map<std::string, GLint> uniformLocations;
    std::unordered_map<std::string, GLint> uniformBufferIndices;

public:

    explicit shader_program(const std::initializer_list<shader>& shaders);

    ~shader_program();

    inline void use() const {
        glUseProgram(handle);
    }

    void addUniform(const std::string& name);

    void addUniformBuffer(const std::string& name);

    // Non-arithmetic types (eg vector and matrix types) are passed by-const-reference
    // template<typename T, std::enable_if_t<!std::is_arithmetic<T>::value, bool> = true>
    // void setUniform(const std::string& name, const T& value) const;

    // Arithmetic types are passed by-value
    template<typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
    void setUniform(const std::string& name, T value) const;

    GLint getUniformLocation(const std::string& name) const;

    void bindUniformBuffer(const std::string& name, uint32_t binding, const buffer& buffer, intptr_t offset, size_t size) const;

};

}  // namespace ogu
