#pragma once

#include <cassert>
#include <vector>

#include "buffer.h"


namespace ogu {

struct vertex_attrib_description {
    GLuint location;
    GLint size;
    GLenum type;
    GLsizeiptr offset;

    bool integer, normalized;

    vertex_attrib_description(GLuint location, GLint size, GLenum type,
        GLsizeiptr offset, bool integer = false, bool normalized = false) :
        location(location), size(size), type(type), offset(offset),
        integer(integer), normalized(normalized)
    { }
}; 

struct vertex_buffer_binding {
    const buffer& buf;

    std::vector<vertex_attrib_description> attribs;

    uint32_t stride;
    bool instanced;

    vertex_buffer_binding(const buffer& buf,
        const std::vector<vertex_attrib_description>& attribs,
        uint32_t stride, bool instanced = false) :
        buf(buf), attribs(attribs), stride(stride), instanced(instanced)
    { }
};

class vertex_array {

public:

    explicit vertex_array(const std::vector<vertex_buffer_binding>& bindings);

    vertex_array(vertex_array&&);

    ~vertex_array();

    vertex_array(const vertex_array&) = delete;

    vertex_array& operator=(const vertex_array&) = delete;
    vertex_array& operator=(vertex_array&&) = delete;

    void bind() const;

private:

    GLuint _handle;

};

inline void vertex_array::bind() const {
    glBindVertexArray(_handle);
}

} // namespace ogu