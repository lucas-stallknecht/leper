#pragma once

#include <cstdint>
#include <array>

namespace leper {

    using Entity = uint32_t;

    constexpr Entity MAX_ENTITIES = 1024u;
    constexpr uint8_t MAX_COMPONENTS = 32u;

    template <typename T>
    using ComponentStorageArray = std::array<T, MAX_ENTITIES>;

} // namespace leper
