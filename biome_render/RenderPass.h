#pragma once

#include <pch.h>
#include "biome_render/RenderUnit.h"

using namespace biome;
using namespace biome::rhi;
using namespace biome::data;

namespace biome::render
{
    struct RenderPass
    {
        ShaderResourceLayoutHandle m_resourceLayout { Handle_Null };
        TextureHandle m_renderTargets[8] { Handle_Null };
        DescriptorHeapHandle m_descriptorHeap { Handle_Null };

        biome::rhi::Rectangle scissorRect;
        biome::rhi::Viewport viewport;

        Vector<TextureHandle> m_textureDependencies {};
        Vector<BufferHandle> m_bufferDependencies {};

        Vector<TextureHandle> m_outputTextures {};
        Vector<BufferHandle> m_outputBuffers {};

        Vector<RenderUnit>* m_pRenderUnits { nullptr };
    };
}
