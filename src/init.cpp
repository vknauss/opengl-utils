#include "opengl-utils.hpp"

#include <stdexcept>


void ogu::init() {
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("Failed to initialize.");
}