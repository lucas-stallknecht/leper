#ifndef OBJ_LOADING_H
#define OBJ_LOADING_H

#include <cstdint>
#include <optional>
#include <string>

#include <glm/glm.hpp>

#include "leper/leper_geometry_types.h"

namespace leper {

    struct FaceVertexTriplet {
        uint32_t v, vt, vn;
    };

    std::optional<FaceVertexTriplet> parseFaceTriplet(const std::string& token);
    std::optional<std::vector<Vertex>> load_obj_geometry(const std::string& file_name);

} // namespace leper

#endif // OBJ_LOADING_H
