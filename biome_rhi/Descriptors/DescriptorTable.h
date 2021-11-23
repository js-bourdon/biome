#pragma once

#include "biome_rhi/Resources/ResourceHandles.h"

namespace biome::rhi
{
    struct DescriptorTable
    {
        DescriptorHeapHandle    m_heapHdl;
        uint32_t                m_offset;
        uint32_t                m_count;
    };
}
