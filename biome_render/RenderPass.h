#pragma once

#include "biome_core/Handle/Handle.h"
#include "biome_render/RenderUnit.h"

using namespace biome;
using namespace biome::rhi;
using namespace biome::data;

namespace biome::render
{
    struct RenderPass
    {
        ShaderResourceLayoutHandle m_resourceLayout { Handle_NULL };
        TextureHandle m_renderTargets[8] { Handle_NULL };
        DescriptorHeapHandle m_descriptorHeap { Handle_NULL };

        biome::rhi::Rectangle scissorRect;
        biome::rhi::Viewport viewport;

        Vector<TextureHandle> m_textureDependencies {};
        Vector<BufferHandle> m_bufferDependencies {};

        Vector<TextureHandle> m_outputTextures {};
        Vector<BufferHandle> m_outputBuffers {};

        Vector<RenderUnit>* m_pRenderUnits { nullptr };
    };
}
