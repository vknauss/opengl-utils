#include "opengl-utils.hpp"

#include <utility>


namespace ogu {

buffer::buffer() {
    glGenBuffers(1, &_handle);
}

buffer::~buffer() {
    glDeleteBuffers(1, &_handle);
}

buffer::buffer(buffer&& b) :
    _handle(std::move(b._handle)) {
    b._handle = 0;
}

void buffer::bind(GLenum target) const {
    glBindBuffer(target, _handle);
}

void buffer::allocate(GLsizeiptr size, GLenum usage, const void* data) const {
    bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

void buffer::write(GLintptr offset, GLsizeiptr size, const void* data) const {
    bind(GL_ARRAY_BUFFER);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void* buffer::map(GLenum usage) const {
    bind(GL_ARRAY_BUFFER);
    return glMapBuffer(GL_ARRAY_BUFFER, usage);
}

void* buffer::map_range(GLintptr offset, GLsizeiptr size, GLbitfield access_flags) const {
    bind(GL_ARRAY_BUFFER);
    return glMapBufferRange(GL_ARRAY_BUFFER, offset, size, access_flags);
}

void buffer::unmap() const {
    bind(GL_ARRAY_BUFFER);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

};  // namespace ogu