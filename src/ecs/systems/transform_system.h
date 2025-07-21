#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "leper/leper_ecs_types.h"
#include "../ecs.h"

namespace leper {

    class TransformSystem {
      public:
        explicit TransformSystem(ECS* ecs);
        void update();
        void translate(Entity entity, const glm::vec3& delta);
        void scale(Entity entity, const glm::vec3& factor);
        void rotate(Entity entity, const glm::quat& delta_rotation);
        void rotate_euler(Entity entity, const glm::vec3& euler_radians);

      private:
        ECS* ecs_;
    };

} // namespace leper
