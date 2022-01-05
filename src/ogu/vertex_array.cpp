#include "vertex_array.h"


namespace ogu
{

vertex_array::vertex_array(const std::vector<vertex_buffer_binding>& bindings) {
    glGenVertexArrays(1, &_handle);

    bind();

    for (const auto& b : bindings) {
        b.buf.bind(GL_ARRAY_BUFFER);

        for (const auto& a : b.attribs) {
            if (a.integer) {
                glVertexAttribIPointer(a.location, a.size, a.type, b.stride, (void*) a.offset);
            } else {
                glVertexAttribPointer(a.location, a.size, a.type,
                    a.normalized? GL_TRUE : GL_FALSE, a.stride, (void*) a.offset);
            }
            if (b.instanced)
                glVertexAttribDivisor(a.location, 1);
            glEnableVertexAttribArray(a.location);
        }
    }
}

vertex_array::vertex_array(vertex_array&& va) : _handle(va._handle) {
    va._handle = 0;
}

vertex_array::~vertex_array() {
    glDeleteVertexArrays(1, &_handle);
}

} // namespace ogu
