#pragma once

#include <pch.h>

using namespace biome::rhi;
using namespace biome::math;

namespace biome::render
{
    struct RenderUnit
    {
        GfxPipelineHandle m_psoHdl { Handle_NULL };
        BufferHandle m_indexBufferHdl { Handle_NULL };
        BufferHandle m_vertexBufferHdl { Handle_NULL };
        Matrix4x4 m_world {};
    };
}