#pragma once

#include <cstdint>
#include <glad/glad.h>

namespace leper {

    using MaterialId = uint8_t;

    struct MeshGlObjetcs {
        GLuint vao;
        GLuint ebo;
    };

} // namespace leper
