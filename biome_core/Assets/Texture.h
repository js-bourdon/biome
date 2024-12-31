#pragma once

#include <stdint.h>

namespace biome
{
    namespace asset
    {
        enum class TextureFormat : int32_t
        {
            Undefined = -1,
            R_Float,
            RG_Float,
            RBG_Float,
            RBGA_Float,
            BC1,
            BC2,
            BC3,
            Count
        };

        struct Texture
        {
            uint64_t        m_byteSize;
            uint64_t        m_byteOffset;
            uint32_t        m_pixelWidth;
            uint32_t        m_pixelHeight;
            TextureFormat   m_format;
        };
    }
}