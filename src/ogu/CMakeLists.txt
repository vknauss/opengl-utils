target_sources(opengl-utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer_texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vertex_array.cpp)

target_include_directories(opengl-utils PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})