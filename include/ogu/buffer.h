#pragma once

#include <cassert>
#include <cstdint>
#include <GL/glew.h>


namespace ogu {

class buffer {
private:

    GLuint _handle;
    size_t _size;

public:

    explicit buffer(size_t size);

    buffer(buffer&& b);

    ~buffer();

    inline void bind(GLenum target) const;

    inline size_t size() const;

    inline const GLuint& handle() const;

    template<typename Fn>
    inline void write(intptr_t offset, size_t size, const Fn& fn) const;

};

void buffer::bind(GLenum target) const {
    glBindBuffer(target, _handle);
}

size_t buffer::size() const {
    return _size;
}

const GLuint& buffer::handle() const {
    return _handle;
}

template<typename Fn>
void buffer::write(intptr_t offset, size_t size, const Fn& fn) const {
    size = (size == 0) ? _size : size;
    void* pBufferData;
    bind(GL_COPY_WRITE_BUFFER); // using GL_COPY_WRITE_BUFFER because why not
    GLbitfield flags = GL_MAP_WRITE_BIT;
    if (offset == 0 && size == _size) {
        flags |= GL_MAP_INVALIDATE_BUFFER_BIT;
    } else {
        flags |= GL_MAP_INVALIDATE_RANGE_BIT;
    }
    pBufferData = glMapBufferRange(GL_COPY_WRITE_BUFFER, offset, size, flags);
    assert(pBufferData);
    fn(pBufferData);
    bind(GL_COPY_WRITE_BUFFER); // rebind the buffer in case the user did some shenanigans
    glUnmapBuffer(GL_COPY_WRITE_BUFFER);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

}  // namespace ogu