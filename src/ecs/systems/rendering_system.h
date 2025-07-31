#pragma once

#include "../ecs.h"
#include "../../renderer/renderer.h"
#include "leper/leper_ecs_types.h"

namespace leper {

    class RenderingSystem {
      public:
        RenderingSystem(ECS* ecs, Renderer* renderer);
        void draw(uint16_t width, uint16_t height, Entity camera,
                  const std::vector<glm::vec2>& trailPoints);

      private:
        void setup_shaders();
        void draw_shadow_map_(const glm::mat4& light_matrix);
        void cleanup();

        ECS* ecs_;
        Renderer* renderer_;
    };

} // namespace leper
