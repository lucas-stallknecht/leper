#pragma once

#include <cstdint>
#include <vector>

#include "leper/leper_geometry_types.h"

namespace leper {

    struct MeshComponent {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

} // namespace leper
