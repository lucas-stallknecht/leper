#pragma once

#include <bitset>
#include <cstdint>

namespace leper {

    using Entity = uint32_t;
    constexpr Entity MAX_ENTITIES = 1024u;

    constexpr uint8_t MAX_COMPONENTS = 32u;
    using Signature = std::bitset<MAX_COMPONENTS>;

} // namespace leper
