#include <cmath>
#include <glm/glm.hpp>

namespace leper {

    struct Vertex {
        glm::vec3 position;
        float_t uv_1;
        glm::vec3 normal;
        float_t uv_2;
    };

} // namespace leper
