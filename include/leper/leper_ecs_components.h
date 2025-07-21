#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "leper/leper_common_types.h"

namespace leper {

    using MeshComponent = Mesh;
    using BasicMaterialComponent = BasicMaterial;

    struct TransformComponent {
        Transform transform = {};
        glm::mat4 model = glm::identity<glm::mat4>();

        bool is_dirty = false;
    };

} // namespace leper
