#pragma once

#include <GL/glew.h>

#include <cstdint>
#include <string>
#include <unordered_map>

#include "buffer.h"


namespace ogu {

class Shader {

    friend class ShaderProgram;

    GLuint handle;

public:

    enum Type {
        VERTEX = GL_VERTEX_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER
    };

    Shader(const std::string& filename, Type type);
    Shader(Shader&&);

    ~Shader();

};

class ShaderProgram {
private:

    GLuint handle;

    std::unordered_map<std::string, GLint> uniformLocations;
    std::unordered_map<std::string, GLint> uniformBufferIndices;

public:

    explicit ShaderProgram(const std::initializer_list<Shader>& shaders);

    ~ShaderProgram();

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
