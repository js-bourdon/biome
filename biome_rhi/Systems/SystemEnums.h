#pragma once

namespace biome::rhi
{
    enum class PrimitiveTopology
    {
        Unknown = -1,
        TriangleList,
        Count
    };

    enum class CommandType
    {
        Unknown = -1,
        Graphics,
        AsyncCompute,
        Copy,
        Count
    };
}