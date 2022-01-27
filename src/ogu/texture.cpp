#include "texture.h"

#include <cassert>

#include <iostream>
#include <stdexcept>
#include <tuple>


namespace ogu {

// Allowed formats
bool Texture::Format::validate(const Format &format) {
    if (format.components == 0 || format.components > 4)
        return false;
    // While ogu does have internal formats that have other numbers of bits-per-component,
    // I'm not going to implement them to make things simpler
    if (format.bitsPerComponent != 8 && format.bitsPerComponent != 16 && format.bitsPerComponent != 32)
        return false;
    // No 8-bit float
    if (format.bitsPerComponent == 8 && format.isFloatingPoint)
        return false;
    // No 32-bit normalized types
    if (format.bitsPerComponent == 32 && format.isNormalized)
        return false;
    // No unsigned or normalized floating-point types
    if (format.isFloatingPoint && (!format.isSigned || format.isNormalized))
        return false;
    return true;
}



static auto getFormat(const Texture::Format& format) {
    // components = 1, 2, 3, 4, store components-1 as 2 bits
    // bitsPerComponent = 8, 16, or 32, code as another 2 bits
    // use bitsPerComponent/8 - 1, just ignore when =2 (would be 24 bits)
    // 1 bit each for floating point, normalized, signed
    // total: 7 bits

    uint8_t key = 0;
    key |= (uint8_t) (format.components - 1) << 5;
    key |= (uint8_t) (format.bitsPerComponent/8 - 1) << 3;
    key |= (format.isFloatingPoint ? 1u : 0u) << 2;
    key |= (format.isNormalized ? 1u : 0u) << 1;
    key |= (format.isSigned ? 1u : 0u) << 0;

    // index into LUT to get formats
    static const GLint internalFormats[128] = {
        // 1 component
        // 8 bits-per-component
        GL_R8UI, GL_R8I, GL_R8, GL_R8_SNORM, 0, 0, 0, 0,
        // 16 bits-per-component
        GL_R16UI, GL_R16I, GL_R16, GL_R16_SNORM, 0, GL_R16F, 0, 0,
        // 24 bits-per-component
        0, 0, 0, 0, 0, 0, 0, 0,
        // 32 bits-per-component
        GL_R32UI, GL_R32I, 0, 0, 0, GL_R32F, 0, 0,
        // 2 components
        // 8 bits-per-component
        GL_RG8UI, GL_RG8I, GL_RG8, GL_RG8_SNORM, 0, 0, 0, 0,
        // 16 bits-per-component
        GL_RG16UI, GL_RG16I, GL_RG16, GL_RG16_SNORM, 0, GL_RG16F, 0, 0,
        // 24 bits-per-component
        0, 0, 0, 0, 0, 0, 0, 0,
        // 32 bits-per-component
        GL_RG32UI, GL_RG32I, 0, 0, 0, GL_RG32F, 0, 0,
        // 3 components
        // 8 bits-per-component
        GL_RGB8UI, GL_RGB8I, GL_RGB8, GL_RGB8_SNORM, 0, 0, 0, 0,
        // 16 bits-per-component
        GL_RGB16UI, GL_RGB16I, GL_RGB16, GL_RGB16_SNORM, 0, GL_RGB16F, 0, 0,
        // 24 bits-per-component
        0, 0, 0, 0, 0, 0, 0, 0,
        // 32 bits-per-component
        GL_RGB32UI, GL_RGB32I, 0, 0, 0, GL_RGB32F, 0, 0,
        // 4 components
        // 8 bits-per-component
        GL_RGBA8UI, GL_RGBA8I, GL_RGBA8, GL_RGBA8_SNORM, 0, 0, 0, 0,
        // 16 bits-per-component
        GL_RGBA16UI, GL_RGBA16I, GL_RGBA16, GL_RGBA16_SNORM, 0, GL_RGBA16F, 0, 0,
        // 24 bits-per-component
        0, 0, 0, 0, 0, 0, 0, 0,
        // 32 bits-per-component
        GL_RGBA32UI, GL_RGBA32I, 0, 0, 0, GL_RGBA32F, 0, 0,
    };

    // index into LUT to get type, using lower 5 bits only
    static const GLenum componentTypes[32] = {
        // 8 bits-per-component
        GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_BYTE, 0, 0, 0, 0,
        // 16 bits-per-component
        GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_SHORT, GL_SHORT, 0, GL_HALF_FLOAT, 0, 0,
        // 24 bits-per-component
        0, 0, 0, 0, 0, 0, 0, 0,
        // 32 bits-per-component
        GL_UNSIGNED_INT, GL_INT, 0, 0, 0, GL_FLOAT, 0, 0,
    };

    GLint internalFormat = internalFormats[key&0x7f];  // in case of bad input, won't overflow the array by having the most-significant bit set
    GLenum componentType = componentTypes[key&0x1F];  // lower 5 bits to ignore num components
    GLenum pixelFormat;

    if (format.isNormalized || format.isFloatingPoint) {
        switch (format.components) {
        case 1: pixelFormat = GL_RED;
            break;

        case 2: pixelFormat = GL_RG;
            break;
        
        case 3: 
            if (format.isBGR) {
                pixelFormat = GL_BGR;
            } else {
                pixelFormat = GL_RGB;
            }
            break;
        
        case 4: 
            if (format.isBGR) {
                pixelFormat = GL_BGRA;
            } else {
                pixelFormat = GL_RGBA;
            }
            break;
        }
    } else {
        switch (format.components) {
        case 1: pixelFormat = GL_RED_INTEGER;
            break;

        case 2: pixelFormat = GL_RG_INTEGER;
            break;
        
        case 3: 
            if (format.isBGR) {
                pixelFormat = GL_BGR_INTEGER;
            } else {
                pixelFormat = GL_RGB_INTEGER;
            }
            break;
        
        case 4: 
            if (format.isBGR) {
                pixelFormat = GL_BGRA_INTEGER;
            } else {
                pixelFormat = GL_RGBA_INTEGER;
            }
            break;
        }
    }

    return std::make_tuple(internalFormat, pixelFormat, componentType);
}

static auto getFormat(const Texture::DepthFormat& format) {
    GLint internalFormat = 0;
    GLenum pixelFormat, componentType;

    switch (format.depthBits) {
    case 32: {
        if (format.hasStencilComponent) {
            internalFormat = GL_DEPTH32F_STENCIL8;
            pixelFormat = GL_DEPTH_STENCIL;
            componentType = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        } else {
            internalFormat = GL_DEPTH_COMPONENT32F;
            pixelFormat = GL_DEPTH_COMPONENT;
            componentType = GL_FLOAT;
        }
        break;
    }
    case 24: {
        if (format.hasStencilComponent) {
            internalFormat = GL_DEPTH24_STENCIL8;
            pixelFormat = GL_DEPTH_STENCIL;
            componentType = GL_UNSIGNED_INT_24_8;
        } else {
            internalFormat = GL_DEPTH_COMPONENT24;
            pixelFormat = GL_DEPTH_COMPONENT;
            componentType = GL_UNSIGNED_INT;
        }
        break;
    }
    case 16: {
        internalFormat = GL_DEPTH_COMPONENT16;
        pixelFormat = GL_DEPTH_COMPONENT;
        componentType = GL_UNSIGNED_SHORT;
        break;
    }
    }

    return std::make_tuple(internalFormat, pixelFormat, componentType);
}

static Texture::Format makeFormat(uint32_t components, Texture::ChannelFormat channelFormat, uint32_t extraFlags) {
    switch(channelFormat) {
    case Texture::U8:
        return {.components=components,.bitsPerComponent=8,.isSigned=false,.isNormalized=true,.isFloatingPoint=false,.isBGR=(bool)(extraFlags & Texture::BGR_BIT)};
    case Texture::U16:
        return {.components=components,.bitsPerComponent=16,.isSigned=false,.isNormalized=true,.isFloatingPoint=false,.isBGR=(bool)(extraFlags & Texture::BGR_BIT)};
    case Texture::FLOAT16:
        return {.components=components,.bitsPerComponent=16,.isSigned=true,.isNormalized=false,.isFloatingPoint=true,.isBGR=(bool)(extraFlags & Texture::BGR_BIT)};
    case Texture::FLOAT32:
        return {.components=components,.bitsPerComponent=16,.isSigned=true,.isNormalized=false,.isFloatingPoint=true,.isBGR=(bool)(extraFlags & Texture::BGR_BIT)};
    }
    assert(0);
    return {};
}

Texture::Texture(uint32_t width, uint32_t height, uint32_t components, Texture::ChannelFormat format, void* pPixelData, uint32_t extraFlags) :
        Texture(width, height, 1, DIMENSION_2D, makeFormat(components, format, extraFlags), pPixelData) {
}

Texture::Texture(uint32_t width, uint32_t height, uint32_t depth, Dimension dimension, Format format, void* pPixelData) :
        Texture(dimension, format) {
    writePixels(width, height, depth, pPixelData);
}

Texture::Texture(Dimension dimension, Format format) :
        width(0), height(0), depth(0), dimension(dimension) /*, format(format)*/ {
    glGenTextures(1, &handle);

    std::tie(internalFormat, pixelFormat, componentType) = getFormat(format);

    switch (dimension) {
    case DIMENSION_1D:
        target = GL_TEXTURE_1D;
        break;
    case DIMENSION_2D:
        target = GL_TEXTURE_2D;
        break;
    case DIMENSION_3D:
        target = GL_TEXTURE_3D;
        break;
    }
}

Texture::Texture(Dimension dimension, DepthFormat format) :
        width(0), height(0), depth(0), dimension(dimension) /*, format(format)*/ {
    glGenTextures(1, &handle);

    std::tie(internalFormat, pixelFormat, componentType) = getFormat(format);

    switch (dimension) {
    case DIMENSION_1D:
        target = GL_TEXTURE_1D;
        break;
    case DIMENSION_2D:
        target = GL_TEXTURE_2D;
        break;
    case DIMENSION_3D:
        target = GL_TEXTURE_3D;
        break;
    }
}

Texture::~Texture() {
    glDeleteTextures(1, &handle);
}

void Texture::bind(uint32_t index) const {
    glActiveTexture(GL_TEXTURE0+index);
    glBindTexture(target, handle);
}

void Texture::setFilterMode(FilterMode magFilter, FilterMode minFilter, FilterMode mipmap) const {
    glBindTexture(target, handle);
    GLint mag = GL_LINEAR, min = GL_LINEAR;
    switch (magFilter) {
    case FilterMode::LINEAR:
        mag = GL_LINEAR;
        break;
    case FilterMode::NEAREST:
        mag = GL_NEAREST;
        break;
    case FilterMode::DISABLED:
        throw std::invalid_argument("Mag filter cannot be disabled.");
    }
    switch (minFilter) {
    case FilterMode::LINEAR: {
        switch (mipmap) {
        case FilterMode::LINEAR:
            min = GL_LINEAR_MIPMAP_LINEAR;
            break;
        case FilterMode::NEAREST:
            min = GL_LINEAR_MIPMAP_NEAREST;
            break;
        case FilterMode::DISABLED:
            min = GL_LINEAR;
            break;
        }
        break;
    }
    case FilterMode::NEAREST: {
        switch (mipmap) {
        case FilterMode::LINEAR:
            min = GL_NEAREST_MIPMAP_LINEAR;
            break;
        case FilterMode::NEAREST:
            min = GL_NEAREST_MIPMAP_NEAREST;
            break;
        case FilterMode::DISABLED:
            min = GL_NEAREST;
            break;
        }
        break;
    }
    case FilterMode::DISABLED:
        throw std::invalid_argument("Min filter cannot be disabled.");
    }
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min);
}

void Texture::setEdgeMode(EdgeMode mode, float* borderColor) const {
    glBindTexture(target, handle);
    GLint edge = GL_REPEAT;
    switch (mode) {
    case EdgeMode::BORDER:
        edge = GL_CLAMP_TO_BORDER;
        break;
    case EdgeMode::CLAMP:
        edge = GL_CLAMP_TO_EDGE;
        break;
    case EdgeMode::REPEAT:
        edge = GL_REPEAT;
        break;
    }
    glTexParameteri(target, GL_TEXTURE_WRAP_S, edge);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, edge);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, edge);
    if (borderColor) {
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
}

void Texture::generateMipmaps() const {
    glBindTexture(target, handle);
    glGenerateMipmap(target);
}

void Texture::writePixels(uint32_t width, uint32_t height, uint32_t depth, void *pPixelData) {
    this->width = width;
    this->height = height;
    this->depth = depth;

    glBindTexture(target, handle);
    
    switch (dimension) {
    case DIMENSION_1D:
        glTexImage1D(target, 0, internalFormat, width, 0, pixelFormat, componentType, pPixelData);
        break;
    case DIMENSION_2D:
        glTexImage2D(target, 0, internalFormat, width, height, 0, pixelFormat, componentType, pPixelData);
        break;
    case DIMENSION_3D:
        glTexImage3D(target, 0, internalFormat, width, height, depth, 0, pixelFormat, componentType, pPixelData);
        break;
    }
}

};