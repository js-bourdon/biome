#pragma once

#include "biome_core/DataStructures/StaticArray.h"

namespace biome::rhi
{
    namespace descriptors
    {
        enum class InputLayoutSemantic
        {
            Position,
            Normal,
            Tangent,
            UV,
            Float2,
            Float3,
            Float4,
            EnumCount
        };

        struct InputLayoutElement
        {
            InputLayoutSemantic             Semantic;
            uint32_t                        SemanticIndex;
            uint32_t                        Slot;
            biome::rhi::descriptors::Format Format;
        };

        struct InputLayoutDesc
        {
            data::StaticArray<InputLayoutElement> Elements {};
        };
    }
}