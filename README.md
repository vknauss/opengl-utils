# opengl-utils

Minimal OpenGL utility wrappers, designed to give more of a C++ feel to the way OpenGL code can be written without sacrificing much of the flexibility of the C API. Extremely minimal at the moment, but enough to reasonably get started with basic rendering tasks. The code styles are a bit jumbled at the moment. Some of this code is adapted from older code I wrote, then I had a bit of a snake_case kick and changed a bunch of names to resemble the STL in style moreso, so there are some inconsistencies there I'll have to work out.

The headers that should be used are in include/ogu. This is intended to be used as a static library, and alongside GLEW.
