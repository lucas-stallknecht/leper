#pragma once

#include <bitset>
#include <cstdint>

namespace leper {

    using Entity = uint32_t;
    constexpr Entity MAX_ENTITIES = 1024u;

    using ComponentType = uint8_t;
    constexpr ComponentType MAX_COMPONENTS = 32u;

    using Signature = std::bitset<MAX_COMPONENTS>;

} // namespace leper
