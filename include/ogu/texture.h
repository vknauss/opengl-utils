#pragma once

#include <GL/glew.h>


namespace ogu {

class Texture {

public:

    // i.e, 1D, 2D, 3D but sadly cannot start with a number
    enum Dimension {
        DIMENSION_1D, DIMENSION_2D, DIMENSION_3D
    };

    struct Format {
        uint32_t components;
        uint32_t bitsPerComponent;
        bool isSigned;
        bool isNormalized;
        bool isFloatingPoint;
        bool isBGR;

        static bool validate(const Format& format);
    };

    struct DepthFormat {
        uint32_t depthBits;
        bool hasStencilComponent;
    };

    enum ChannelFormat {
        U8,
        U16,
        FLOAT16,
        FLOAT32
    };

    enum FilterMode {
        NEAREST,
        LINEAR,
        DISABLED
    };

    enum EdgeMode {
        CLAMP,
        REPEAT,
        BORDER
    };

    enum ExtraFlags {
        BGR_BIT = 1<<0
    };

    // Backwards-compatible constructor for basic 2D textures
    Texture(uint32_t width, uint32_t height, uint32_t components, ChannelFormat format, void* pPixelData = nullptr, uint32_t extraFlags = 0);

    // Flexible constructor for a wide variety of texture types
    Texture(uint32_t width, uint32_t height, uint32_t depth, Dimension dimension, Format format, void* pPixelData = nullptr);

    // Uninitialized texture, when you know you will call writePixels later
    Texture(Dimension dimension, Format format);

    // Uninitialized texture, when you know you will call writePixels later (likely with null value to allocate storage)
    Texture(Dimension dimension, DepthFormat format);
    
    Texture(Texture&& t) { }

    ~Texture();

    inline uint32_t getWidth() const {
        return width;
    }

    inline uint32_t getHeight() const {
        return height;
    }

    inline uint32_t getDepth() const {
        return height;
    }

    void bind(uint32_t index) const;

    void setFilterMode(FilterMode magFilter, FilterMode minFilter, FilterMode mipmap) const;

    // @param borderColor should be a pointer to the beginning of an array of 4 floats, or nullptr
    void setEdgeMode(EdgeMode mode, float* borderColor = nullptr) const;

    void writePixels(uint32_t width, uint32_t height, uint32_t depth, void* pPixelData);

    void generateMipmaps() const;


private:

    GLuint handle;

    uint32_t width, height, depth;
    GLenum target;
    GLint internalFormat;
    GLenum pixelFormat;
    GLenum componentType;
    Dimension dimension;
    //Format format;

};

}  // namespace ogu