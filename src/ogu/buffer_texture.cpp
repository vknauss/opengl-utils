#include "buffer_texture.h"

#include <cstring>
#include <utility>


namespace ogu {

buffer_texture::buffer_texture(buffer_texture&& b) :
        _handle(std::move(b._handle)),
        _buffer(std::move(b._buffer)) { }

// Make sure the format will translate to one of the supported formats in the table at
// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexBuffer.xhtml
bool buffer_texture::Format::validate(const Format& format) {
    // Format must have at least one and at most 4 components
    if (format.components == 0 || format.components > 4)
        return false;
    // Each component must be 8, 16, or 32 bits
    if (format.bitsPerComponent != 8 && format.bitsPerComponent != 16 && format.bitsPerComponent != 32)
        return false;
    /*
    // 3-component are extra strict, per the table only GL_RGB32* supported
    if (format.components == 3 && format.bitsPerComponent != 32)
        return false;
    */
    // 3-component textures only allowed in 4.x or with ARB_texture_buffer_object_rgb32, just disallow them
    if (format.components == 3) {
        return false;
    }
    // 32-bit formats can't be normalized
    if (format.bitsPerComponent == 32 && format.isNormalized)
        return false;
    // 8-bit formats can't be floating point
    if (format.bitsPerComponent == 8 && format.isFloatingPoint)
        return false;
    // Floating-point formats cannot unsigned
    if (format.isFloatingPoint && !format.isSigned)
        return false;
    // Normalized formats must be unsigned
    if (format.isNormalized && format.isSigned)
        return false;
    
    return true;
}

static GLenum getInternalFormat(const buffer_texture::Format& format) {
    // No error checking, assumed the caller has passed a valid format
    switch (format.components) {
    case 1: {
        switch (format.bitsPerComponent) {
        case 8: {
            if (format.isNormalized) {
                return GL_R8;
            } else if (format.isSigned) {
                return GL_R8I;
            } else {
                return GL_R8UI;
            }
        }
        case 16: {
            if (format.isFloatingPoint) {
                return GL_R16F;
            } else if (format.isNormalized) {
                return GL_R16;
            } else if (format.isSigned) {
                return GL_R16I;
            } else {
                return GL_R16UI;
            }
        }
        case 32: {
            if (format.isFloatingPoint) {
                return GL_R32F;
            } else if (format.isSigned) {
                return GL_R32I;
            } else {
                return GL_R32UI;
            }
        }
        }
        break;
    }
    case 2: {
        switch (format.bitsPerComponent) {
        case 8: {
            if (format.isNormalized) {
                return GL_RG8;
            } else if (format.isSigned) {
                return GL_RG8I;
            } else {
                return GL_RG8UI;
            }
        }
        case 16: {
            if (format.isFloatingPoint) {
                return GL_RG16F;
            } else if (format.isNormalized) {
                return GL_RG16;
            } else if (format.isSigned) {
                return GL_RG16I;
            } else {
                return GL_RG16UI;
            }
        }
        case 32: {
            if (format.isFloatingPoint) {
                return GL_RG32F;
            } else if (format.isSigned) {
                return GL_RG32I;
            } else {
                return GL_RG32UI;
            }
        }
        }
        break;
    }
    /*case 3: {
        if (format.isFloatingPoint) {
            return GL_RGBA32F;
        } else if (format.isSigned) {
            return GL_RGBA32I;
        } else {
            return GL_RGBA32UI;
        }
    }*/
    case 4: {
        switch (format.bitsPerComponent) {
        case 8: {
            if (format.isNormalized) {
                return GL_RGBA8;
            } else if (format.isSigned) {
                return GL_RGBA8I;
            } else {
                return GL_RGBA8UI;
            }
        }
        case 16: {
            if (format.isFloatingPoint) {
                return GL_RGBA16F;
            } else if (format.isNormalized) {
                return GL_RGBA16;
            } else if (format.isSigned) {
                return GL_RGBA16I;
            } else {
                return GL_RGBA16UI;
            }
        }
        case 32: {
            if (format.isFloatingPoint) {
                return GL_RGBA32F;
            } else if (format.isSigned) {
                return GL_RGBA32I;
            } else {
                return GL_RGBA32UI;
            }
        }
        }
        break;
    }
    }
    return GL_INVALID_ENUM;
}

buffer_texture::buffer_texture(size_t size, const Format& format, void* pData, uint32_t extraFlags) :
    _buffer(size)
{
    glGenTextures(1, &_handle);
    glBindTexture(GL_TEXTURE_BUFFER, _handle);
    glTexBuffer(GL_TEXTURE_BUFFER, getInternalFormat(format), _buffer.handle());
    if (pData) {
        _buffer.write(0, 0, [&] (void* pBufferData) {
                memcpy(pBufferData, pData, size);
            });
    }
}

buffer_texture::~buffer_texture() {
    glDeleteTextures(1, &_handle);
}

void buffer_texture::bind(uint32_t index) const {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_BUFFER, _handle);
}

}