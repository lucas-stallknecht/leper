#pragma once

#include "leper/leper_ecs_types.h"
#include "leper/leper_rendering_types.h"

namespace leper {
    static ComponentId component_id_counter = 0;
    static MaterialId shader_id_counter = 0;

    template <typename T>
    inline ComponentId get_component_id() {
        static ComponentId id = component_id_counter++;
        return id;
    }

    template <typename T>
    inline MaterialId get_material_id() {
        static MaterialId id = component_id_counter++;
        return id;
    }

} // namespace leper
