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

    struct DirectionalLightComponent {
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float_t intensity = 1.0;
        glm::vec3 direction = glm::vec3(0.0f, 1.0f, 0.0f);
    };

    struct PointLightComponent {
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float_t intensity = 1.0;
        // float_t range = 10.0;
        // PointLightFallof falloff = PointLightFallof::Medium;
    };

} // namespace leper
