#pragma once

#include <unordered_map>

#include "leper/leper_common_types.h"
#include "leper/leper_rendering_types.h"
#include "../../utils/id_utils.h"

namespace leper {

    const std::unordered_map<MaterialId, std::pair<std::string, std::string>> MATERIAL_TO_SHADER_FILES = {
        {get_material_id<BasicMaterial>(), {"basic.vert.glsl", "basic.frag.glsl"}},
    };

}
