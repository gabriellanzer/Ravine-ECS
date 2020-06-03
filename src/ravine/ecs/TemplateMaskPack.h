#ifndef TEMPLATEMASKPACK_H
#define TEMPLATEMASKPACK_H

#include <array>
#include "ComponentStorage.hpp"

namespace rv
{

    template <typename... T>
    struct MaskPack;

    template <>
    struct MaskPack<>
    {
        static constexpr intptr_t mask() { return 0; }
    };

    template <typename H, typename... T>
    struct MaskPack<H, T...>
    {
        static constexpr size_t mask()
        {
            return reinterpret_cast<intptr_t>(ComponentStorage<H>::getInstance()) + MaskPack<T...>::mask();
        }
    };

    template <int N>
    using MaskArray = std::array<intptr_t, N>;

} // namespace rv

#endif