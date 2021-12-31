#include "opengl-utils.hpp"

#include <GL/glew.h>

namespace ogu {

vertex_array::vertex_array() {
    glGenVertexArrays(1, &_handle);
}

vertex_array::vertex_array(vertex_array&& va) :
    _handle(va._handle) {
    va._handle = 0;
}

vertex_array::~vertex_array() {
    glDeleteVertexArrays(1, &_handle);
}

void vertex_array::bind() const {
    glBindVertexArray(_handle);
}

void vertex_array::add_binding(const attrib_binding& binding) const {
    bind();
    binding.vertex_buffer->bind(GL_ARRAY_BUFFER);

    if (binding.integer) {
        glVertexAttribIPointer(binding.location, binding.size, binding.type,
            binding.stride, (void*) binding.offset);
    } else {    
        glVertexAttribPointer(binding.location, binding.size, binding.type,
            binding.normalized ? GL_TRUE : GL_FALSE, binding.stride, (void*) binding.offset);
    }
    glEnableVertexAttribArray(binding.location);
}

};  // namespace ogu