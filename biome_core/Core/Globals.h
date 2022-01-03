#pragma once

#include <cstdint>

namespace biome
{
    namespace core
    {
        template<typename Enum>
        inline Enum CombineFlags(Enum flag0, Enum flag1)
        {
            return static_cast<Enum>(size_t(flag0) | size_t(flag1));
        }

        template<typename EnumType, typename FlagType>
        inline bool HasFlag(EnumType value, FlagType flag)
        {
            auto flagNumericValue = static_cast<std::underlying_type_t<EnumType>>(flag);
            return (static_cast<std::underlying_type_t<EnumType>>(value) & flagNumericValue) == flagNumericValue;
        }
    }
}