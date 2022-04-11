#pragma once

#include "biome_rhi/Resources/ResourceHandles.h"
#include "biome_core/Math/Math.h"

using namespace biome::math;

namespace biome::rhi
{
    struct Viewport;
    struct Rectangle;
    struct DescriptorTable;
    enum class PrimitiveTopology;
    enum class ResourceState;

    namespace commands
    {
        void CloseCommandBuffer(CommandBufferHandle cmdBufferHdl);

        void SetComputeShaderResourceLayout(CommandBufferHandle cmdBufferHdl, ShaderResourceLayoutHandle srlHdl);
        void SetGraphicsShaderResourceLayout(CommandBufferHandle cmdBufferHdl, ShaderResourceLayoutHandle srlHdl);
        void SetComputeConstant(CommandBufferHandle cmdBufferHdl, uint32_t index, uint32_t value, uint32_t destOffsetInValues);
        void SetDescriptorHeaps(CommandBufferHandle cmdBufferHdl, uint32_t count, const DescriptorHeapHandle* pHeapHdls);
        void SetComputeDescriptorTable(CommandBufferHandle cmdBufferHdl, uint32_t index, const DescriptorTable& table);
        void SetGraphicsDescriptorTable(CommandBufferHandle cmdBufferHdl, uint32_t index, const DescriptorTable& table);
        void SetGfxPipeline(CommandBufferHandle cmdBufferHdl, GfxPipelineHandle pipelineHdl);
        void SetComputePipeline(CommandBufferHandle cmdBufferHdl, ComputePipelineHandle pipelineHdl);

        void DispatchCompute(CommandBufferHandle cmdBufferHdl, uint32_t x, uint32_t y, uint32_t z);

        void DrawInstanced(
            CommandBufferHandle cmdBufferHdl,
            uint32_t vertexCountPerInstance,
            uint32_t instanceCount,
            uint32_t startVertex,
            uint32_t startInstance);

        void DrawIndexedInstanced(
            CommandBufferHandle cmdBufferHdl, 
            uint32_t indexCountPerInstance, 
            uint32_t instanceCount, 
            uint32_t startIndex, 
            uint32_t baseVertex, 
            uint32_t startInstance);

        void ClearDepthStencil(CommandBufferHandle cmdBufferHdl, TextureHandle depthStencilHdl);
        void ClearRenderTarget(CommandBufferHandle cmdBufferHdl, TextureHandle renderTargetHdl, Vector4 clearColor);

        // IA
        void SetIndexBuffer(CommandBufferHandle cmdBufferHdl, BufferHandle indexBufferHdl);
        void SetPrimitiveTopology(CommandBufferHandle cmdBufferHdl, PrimitiveTopology topology);
        void SetVertexBuffers(
            CommandBufferHandle cmdBufferHdl, 
            uint32_t startSlot, 
            uint32_t bufferCount, 
            const BufferHandle* pVertexBufferHdls);

        // OM
        void OMSetBlendFactor(CommandBufferHandle cmdBufferHdl, const float* pBlendfactors);
        void OMSetStencilRef(CommandBufferHandle cmdBufferHdl, uint32_t stencilRef);
        void OMSetRenderTargets(
            CommandBufferHandle cmdBufferHdl, 
            uint32_t rtCount, 
            const TextureHandle* pRenderTargets, 
            const TextureHandle* pDepthStencil);

        // RS
        void RSSetScissorRects(CommandBufferHandle cmdBufferHdl, uint32_t count, const Rectangle* pRectangles);
        void RSSetViewports(CommandBufferHandle cmdBufferHdl, uint32_t count, const Viewport* pViewports);

        struct TextureStateTransition
        {
            TextureHandle m_textureHdl { Handle_NULL };
            ResourceState m_before {};
            ResourceState m_after {};
        };

        void ResourceTransition(CommandBufferHandle cmdBufferHdl, const TextureStateTransition* transitions, uint32_t transitionCount);
    }
}