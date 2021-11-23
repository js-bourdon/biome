#pragma once

#include <cstdint>

namespace biome::rhi
{
    namespace descriptors
    {
        enum class SamplerFiltering
        {
            Point,
            Linear,
            Anisotropic,
            Count
        };

        enum class SamplerAddressMode
        {
            Wrap,
            Clamp,
            Count
        };

        struct SamplerStateDesc
        {
            SamplerFiltering    m_Filter;
            SamplerAddressMode  m_AddressU;
            SamplerAddressMode  m_AddressV;
            SamplerAddressMode  m_AddressW;
            float               m_MipBias;
            uint32_t            m_MaxAnisotropy;
        };
    }
}