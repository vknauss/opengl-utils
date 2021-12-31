#pragma once

#include <vector>

#include "buffer.h"

namespace ogu {



class VertexArray {

public:

    struct attrib {
        uint32_t location;
        int size;
        enum {
            PER_VERTEX,
            PER_INSTANCE
        } inputMode = PER_VERTEX;
        enum {
            FLOAT,
            INT,
            UINT
        } type = FLOAT;

        attrib(uint32_t location, int size, decltype(inputMode) inputMode = PER_VERTEX, decltype(type) type = FLOAT) :
            location(location), size(size), inputMode(inputMode), type(type) { }
    };

    VertexArray(size_t vertexBufferSize, size_t indexBufferSize, size_t instanceBufferSize);

    VertexArray(VertexArray&&);

    ~VertexArray();

    void addAttribute(const attrib& attrib);

    void createAttributeBindings() const;

    inline const buffer& getVertexBuffer() const {
        return vertexBuffer;
    }

    inline const buffer& getInstanceBuffer() const {
        return instanceBuffer;
    }

    inline const buffer& getIndexBuffer() const {
        return indexBuffer;
    }

    void enable() const;

    void disable() const;

private:

    GLuint handle;

    std::vector<attrib> attribs;

    buffer vertexBuffer;
    buffer instanceBuffer;
    buffer indexBuffer;

};

} // namespace ogu