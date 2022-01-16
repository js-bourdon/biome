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
        Structured,
        Raw,
        Count
    };

    enum class ResourceState
    {
        Common = 0x0000,
        VertexBuffer = 0x0001,
        ConstantBuffer = 0x0002,
        IndexBuffer = 0x0004,
        RenderTarget = 0x0008,
        UnorderedAccess = 0x0010,
        DepthWrite = 0x0020,
        DepthRead = 0x0040,
        GeometryShaderResource = 0x0080,
        FragmentShaderResource = 0x0100,
        IndirectArgument = 0x0200,
        CopyDestination = 0x0400,
        CopySource = 0x0800,
        Present = 0x1000,
    };
}