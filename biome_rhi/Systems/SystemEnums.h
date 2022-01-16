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

    enum class BufferType
    {
        Unknown = -1,
        Vertex,
        Index,
        Constant,
        Count
    };
}