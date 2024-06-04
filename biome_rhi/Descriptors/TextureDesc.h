#pragma once

#include <cstdint>

#include "Formats.h"
#include "ResourceUsage.h"

namespace biome::rhi
{
    namespace descriptors
    {
        enum class TextureType
        {
            Texture2D,
            DepthStencil,
            EnumCount
        };

        enum class TextureBindFlags
        {
            ShaderResource      = 0x01,
            RenderTarget        = 0x02,
            UnorderedAccessView = 0x04,
        };

        struct TextureDesc
        {
            const char* DebugName { nullptr };
            TextureType Type {};
            Format Format {};
            ResourceUsage Usage {};
            TextureBindFlags BindFlags {};
            uint32_t Width { 0 };
            uint32_t Height { 0 };
            uint32_t MipLevels { 0 };
            uint32_t ArraySize { 0 };
            uint32_t SampleCount { 0 };
        };
    }
}