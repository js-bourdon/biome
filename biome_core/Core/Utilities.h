#pragma once

#include <cstdint>

namespace biome
{
    namespace utils
    {
        template<typename T0, typename T1>
        struct LargestTypeTrait
        {
            using Type = std::conditional<(sizeof(T0) >= sizeof(T1)), T0, T1>::type;
        };

        template<typename T0, typename T1> using LargestType = typename LargestTypeTrait<T0, T1>::Type;
    }
}