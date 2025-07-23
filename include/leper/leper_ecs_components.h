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

    struct CameraComponent {
        glm::mat4 view = glm::identity<glm::mat4>();
        glm::mat4 projection = glm::identity<glm::mat4>();
    };

} // namespace leper
