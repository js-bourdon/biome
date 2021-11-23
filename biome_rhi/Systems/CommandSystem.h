#pragma once

#include "biome_rhi/Resources/ResourceHandles.h"

namespace biome::rhi
{
    struct Viewport;
    struct Rectangle;
    struct DescriptorTable;
    enum class PrimitiveTopology;

    namespace commands
    {
        void CloseCommandBuffer(CommandBufferHandle cmdBufferHdl);

        void SetComputeShaderResourceLayout(ShaderResourceLayoutHandle srlHdl);
        void SetGraphicsShaderResourceLayout(ShaderResourceLayoutHandle srlHdl);
        void SetComputeConstant(uint32_t index, uint32_t value, uint32_t destOffsetInValues);
        void SetDescriptorHeaps(uint32_t count, const DescriptorHeapHandle* pHeapHdls);
        void SetComputeDescriptorTable(uint32_t index, const DescriptorTable& table);
        void SetGraphicsDescriptorTable(uint32_t index, const DescriptorTable& table);
        void SetPipeline(PipelineHandle pipelineHdl);

        void DispatchCompute(CommandBufferHandle cmdBufferHdl, uint32_t x, uint32_t y, uint32_t z);

        void DrawIndexedInstanced(
            CommandBufferHandle cmdBufferHdl,
            uint32_t vertexCountPerInstance,
            uint32_t instanceCount,
            uint32_t startVertex,
            uint32_t startInstance);

        void DrawInstanced(
            CommandBufferHandle cmdBufferHdl, 
            uint32_t indexCountPerInstance, 
            uint32_t instanceCount, 
            uint32_t startIndex, 
            uint32_t baseVertex, 
            uint32_t startInstance);

        void ClearDepthStencil(CommandBufferHandle cmdBufferHdl, TextureHandle depthStencilHdl);
        void ClearRenderTarget(CommandBufferHandle cmdBufferHdl, TextureHandle renderTargetHdl);

        // IA
        void SetIndexBuffer(CommandBufferHandle cmdBufferHdl, IndexBufferHandle indexBufferHdl);
        void SetPrimitiveTopology(CommandBufferHandle cmdBufferHdl, PrimitiveTopology topology);
        void SetVertexBuffers(
            CommandBufferHandle cmdBufferHdl, 
            uint32_t startSlot, 
            uint32_t bufferCount, 
            const VertexBufferHandle* pVertexBufferHdls);

        // OM
        void OMSetBlendFactor(CommandBufferHandle cmdBufferHdl, const float* pBlendfactors);
        void OMSetStencilRef(CommandBufferHandle cmdBufferHdl, uint32_t stencilRef);
        void OMSetRenderTargets(
            CommandBufferHandle cmdBufferHdl, 
            uint32_t rtCount, 
            const TextureHandle* pRenderTargets, 
            const TextureHandle* pDepthStencil);

        // RS
        void RSSetScissorRects(uint32_t count, const Rectangle* pRectangles);
        void RSSetViewports(uint32_t count, const Viewport* pViewports);
    }
}