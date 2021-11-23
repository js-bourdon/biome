#pragma once

#include <cstdint>

namespace biome::rhi
{
    namespace descriptors
    {
        enum class PixelFormat;

        struct SurfaceDesc
        {
            uint32_t    Width;
            uint32_t    Height;
            uint32_t    SampleCount;
            PixelFormat Format;
        };
    }
}