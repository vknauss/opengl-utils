#pragma once

#include <GL/glew.h>

#include <vector>

namespace ogu {

class buffer {
    GLuint _handle;

public:

    buffer();
    ~buffer();

    // move construction allowed
    buffer(buffer&&);

    // copy construction and all assignment not allowed
    buffer(const buffer&) = delete;
    buffer& operator=(const buffer&) = delete;
    buffer& operator=(buffer&&) = delete;

    // bind this buffer to the specified target
    void bind(GLenum target) const;

    // allocate storage by calling glBufferData
    void allocate(GLsizeiptr size, GLenum usage, const void* data = nullptr) const;

    void write(GLintptr offset, GLsizeiptr size, const void* data) const;

    void* map(GLenum access) const;

    void* map_range(GLintptr offset, GLsizeiptr size, GLbitfield access_flags) const;

    void unmap() const;

};

class vertex_array {

public:

    vertex_array();

    vertex_array(vertex_array&&);

    ~vertex_array();

    vertex_array(const vertex_array&) = delete;

    vertex_array& operator=(const vertex_array&) = delete;
    vertex_array& operator=(vertex_array&&) = delete;

    void bind() const;

    struct attrib_binding {
        buffer* vertex_buffer;
        GLuint location;
        GLint size;
        GLenum type;
        GLintptr stride;
        GLintptr offset;
        bool integer, normalized;
    };

    void add_binding(const attrib_binding& binding) const;

private:

    GLuint _handle;

};

};  // namespace ogu