#pragma once

#include <cassert>
#include <vector>

#include "buffer.h"


namespace ogu {

struct vertex_attrib_description {
    GLuint location;
    GLint size;
    GLenum type;
    GLintptr stride;
    GLsizeiptr offset;

    bool integer, normalized;
}; 

struct vertex_buffer_binding {
    const buffer& buf;

    std::vector<vertex_attrib_description> attribs;

    uint32_t stride;
    bool instanced;
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