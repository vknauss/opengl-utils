#include "vertex_array.h"


namespace ogu
{

VertexArray::VertexArray(size_t vertexBufferSize, size_t indexBufferSize, size_t instanceBufferSize) :
        vertexBuffer(vertexBufferSize),
        instanceBuffer(instanceBufferSize),
        indexBuffer(indexBufferSize) {
    glGenVertexArrays(1, &handle);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &handle);
}

void VertexArray::addAttribute(const attrib& attrib) {
    attribs.push_back(attrib);
}

void VertexArray::createAttributeBindings() const {
    std::vector<intptr_t> offsets(attribs.size(), 0);
    intptr_t offsetPerVertex = 0;
    intptr_t offsetPerInstance = 0;
    for (auto i = 0u; i < attribs.size(); ++i) {
        size_t size = 0;
        switch (attribs[i].type) {
        case attrib::FLOAT:
            size = sizeof(GLfloat);
            break;
        case attrib::INT:
            size = sizeof(GLint);
            break;
        case attrib::UINT:
            size = sizeof(GLuint);
            break;
        }
        size = size * attribs[i].size;
        switch (attribs[i].inputMode) {
        case attrib::PER_VERTEX:
            offsets[i] = offsetPerVertex;
            offsetPerVertex += size;
            break;
        case attrib::PER_INSTANCE:
            offsets[i] = offsetPerInstance;
            offsetPerInstance += size;
            break;
        }
    }
    
    glBindVertexArray(handle);
    for (auto i = 0u; i < attribs.size(); ++i) {
        uint32_t divisor = 0;
        size_t stride = 0;
        switch (attribs[i].inputMode) {
        case attrib::PER_VERTEX:
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.handle());
            divisor = 0;
            stride = offsetPerVertex;
            break;
        case attrib::PER_INSTANCE:
            glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer.handle());
            divisor = 1;
            stride = offsetPerInstance;
            break;
        }
        switch (attribs[i].type) {
        case attrib::FLOAT:
            glVertexAttribPointer(attribs[i].location, attribs[i].size, GL_FLOAT, GL_FALSE, stride, (void*) offsets[i]);
            break;
        case attrib::INT:
            glVertexAttribIPointer(attribs[i].location, attribs[i].size, GL_INT, stride, (void*) offsets[i]);
            break;
        case attrib::UINT:
            glVertexAttribIPointer(attribs[i].location, attribs[i].size, GL_UNSIGNED_INT, stride, (void*) offsets[i]);
            break;
        }

        glVertexAttribDivisor(attribs[i].location, divisor);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.handle());
}

void VertexArray::enable() const {
    glBindVertexArray(handle);
    for (const auto& attrib : attribs) {
        glEnableVertexAttribArray(attrib.location);
    }
}

void VertexArray::disable() const {
    for (const auto& attrib : attribs) {
        glDisableVertexAttribArray(attrib.location);
    }
}

} // namespace ogu
