#include "buffer.h"

#include <utility>


namespace ogu {

buffer::buffer(size_t size) :
        _size(size) {
    glGenBuffers(1, &_handle);
    glBindBuffer(GL_COPY_WRITE_BUFFER, _handle);
    glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

buffer::buffer(buffer&& b) :
    _handle(std::move(b._handle)),
    _size(std::move(b._size))
{
    b._handle = 0;
}

buffer::~buffer() {
    glDeleteBuffers(1, &_handle);
}

}  // namespace ogu