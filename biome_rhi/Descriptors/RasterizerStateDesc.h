#pragma once

namespace biome::rhi
{
    namespace descriptors
    {
        enum class CullMode
        {
            None,
            Front,
            Back,
            EnumCount
        };

        enum class Winding
        {
            FrontClockwise,
            FrontCounterClockwise,
            EnumCount
        };

        struct RasterizerStateDesc
        {
            CullMode    CullMode { CullMode::Back };
            Winding     Winding { Winding::FrontClockwise };
            bool        DepthClip { true };
        };
    }
}