#pragma once

#include <cmath>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace leper {

    struct Vertex {
        glm::vec3 position;
        float_t uv_1;
        glm::vec3 normal;
        float_t uv_2;
    };

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::string name;
    };

    struct IMaterial {};
    struct BasicMaterial : public IMaterial {
        glm::vec3 albedo = glm::vec3(1.0f, 1.0f, 1.0f);
    };

    struct Transform {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::quat rotation = glm::identity<glm::quat>();
    };


} // namespace leper
