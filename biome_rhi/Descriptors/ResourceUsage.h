#pragma once

namespace biome::rhi
{
    namespace descriptors
    {
        enum class ResourceUsage
        {
            GPU_ReadWrite,
            GPU_ReadOnly,
            GPU_ReadOnly_CPU_WriteOnly,
            GPU_WriteOnly_CPU_ReadOnly,
            EnumCount
        };
    }
}