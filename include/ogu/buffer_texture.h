#pragma once

#include "buffer.h"


namespace ogu {

class buffer_texture {
private:

    GLuint _handle;

    ogu::buffer _buffer;

public:

    struct Format {
        uint32_t components;
        uint32_t bitsPerComponent;
        bool isSigned;
        bool isNormalized;
        bool isFloatingPoint;

        static bool validate(const Format& format);
    };

    buffer_texture(size_t size, const Format& format, void* pData = nullptr, uint32_t extraFlags = 0);
    buffer_texture(buffer_texture&& t);

    ~buffer_texture();

    inline const buffer& getBuffer() const {
        return _buffer;
    }

    void bind(uint32_t index) const;

};

}  // namespace ogu