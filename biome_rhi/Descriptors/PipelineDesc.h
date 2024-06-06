#pragma once

#include "biome_rhi/Descriptors/InputLayoutDesc.h"
#include "biome_rhi/Descriptors/BlendStateDesc.h"
#include "biome_rhi/Descriptors/DepthStencilStateDesc.h"
#include "biome_rhi/Descriptors/RasterizerStateDesc.h"
#include "biome_rhi/Descriptors/Formats.h"
#include "biome_rhi/Resources/ResourceHandles.h"

namespace biome::rhi
{
    namespace descriptors
    {
        struct GfxPipelineDesc
        {
            static constexpr size_t     s_OMMaxRenderTargetCount = 8;

            ShaderResourceLayoutHandle  ResourceLayout {};
            InputLayoutDesc             InputLayout {};
            ShaderHandle                VertexShader {};
            ShaderHandle                FragmentShader {};
            BlendStateDesc              BlendState {};
            DepthStencilStateDesc       DepthStencilState {};
            RasterizerStateDesc         RasterizerState {};
            Format                      RenderTargetFormats[s_OMMaxRenderTargetCount] {};
            Format                      DepthFormat {};
            uint32_t                    RenderTargetCount { 0 };
        };

        struct ComputePipelineDesc
        {

        };
    }
}
