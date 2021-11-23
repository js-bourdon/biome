#pragma once

#include "biome_rhi/Descriptors/InputLayoutDesc.h"
#include "biome_rhi/Descriptors/BlendStateDesc.h"
#include "biome_rhi/Descriptors/DepthStencilStateDesc.h"
#include "biome_rhi/Descriptors/RasterizerStateDesc.h"
#include "biome_rhi/Descriptors/PixelFormats.h"
#include "biome_rhi/Resources/Resources.h"
#include "biome_rhi/Resources/ResourceHandles.h"

namespace biome::rhi
{
    using namespace resources;

    namespace descriptors
    {
        struct GfxPipelineDesc
        {
            static constexpr size_t     s_OMMaxRenderTargetCount = 8;

            ShaderResourceLayoutHandle  ResourceLayout {};
            InputLayoutDesc             InputLayout {};
            Shader                      VertexShader {};
            Shader                      FragmentShader {};
            BlendStateDesc              BlendState {};
            DepthStencilStateDesc       DepthStencilState {};
            RasterizerStateDesc         RasterizerState {};
            PixelFormat                 RenderTargetFormats[s_OMMaxRenderTargetCount] {};
            PixelFormat                 DepthFormat {};
            uint32_t                    RenderTargetCount { 0 };
        };

        struct ComputePipelineDesc
        {

        };
    }
}
