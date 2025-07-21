#include "leper_common_types.h"

#include <typeindex>
#include <unordered_map>

namespace leper {

    const std::unordered_map<std::type_index, std::pair<std::string, std::string>> MATERIAL_TO_SHADER_FILES = {
        {typeid(BasicMaterial), {"basic.vert.glsl", "basic.frag.glsl"}},
    };

}
