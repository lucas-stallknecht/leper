#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <glm/glm.hpp>

#include "leper/leper_common_types.h"

namespace leper {

    struct FaceVertexTriplet {
        uint32_t v, vt, vn;
    };

    std::optional<FaceVertexTriplet> parseFaceTriplet(const std::string& token);
    std::vector<uint32_t> build_indices_from_vertices(const std::vector<Vertex>& vertices);
    std::optional<Mesh> load_obj_mesh(const std::string& file_name);

} // namespace leper
