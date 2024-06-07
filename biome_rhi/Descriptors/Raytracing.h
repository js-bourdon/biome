#pragma once

namespace biome::rhi::descriptors
{
    struct RayTracingInstanceDesc
    {
        Matrix4x4 m_Transform { math::IdentifyMatrix() };
        BufferHandle m_IndexBuffer { Handle_NULL };
        BufferHandle m_VertexPosBuffer { Handle_NULL };
        bool m_IsOpaque { true };
    };
}