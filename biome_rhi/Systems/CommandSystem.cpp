#include <pch.h>
#include "biome_rhi/Systems/CommandSystem.h"
#include "biome_rhi/Resources/Resources.h"
#include "biome_rhi/Systems/SystemEnums.h"
#include "biome_rhi/Systems/SystemUtils.h"
#include "biome_rhi/Descriptors/Viewport.h"
#include "biome_rhi/Descriptors/Rectangle.h"
#include "biome_core/Core/Globals.h"

using namespace biome;
using namespace biome::rhi;
using namespace biome::rhi::resources;

namespace
{
    static D3D12_PRIMITIVE_TOPOLOGY ToNativePrimitiveTopology(PrimitiveTopology topology)
    {
        switch (topology)
        {
        case PrimitiveTopology::TriangleList:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        default:
            return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        }
    }

    static D3D12_RESOURCE_STATES ToNativeResourceState(ResourceState rscState)
    {
        #define AppendResourceState(biomeName, nativeName) \
            nativeState = core::CombineFlags(nativeState, core::HasFlag(rscState, biomeName) ? nativeName : D3D12_RESOURCE_STATE_COMMON)

        D3D12_RESOURCE_STATES nativeState = D3D12_RESOURCE_STATE_COMMON;

        AppendResourceState(ResourceState::VertexBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        AppendResourceState(ResourceState::ConstantBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        AppendResourceState(ResourceState::IndexBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);
        AppendResourceState(ResourceState::RenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET);
        AppendResourceState(ResourceState::UnorderedAccess, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        AppendResourceState(ResourceState::DepthWrite, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        AppendResourceState(ResourceState::DepthRead, D3D12_RESOURCE_STATE_DEPTH_READ);
        AppendResourceState(ResourceState::GeometryShaderResource, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        AppendResourceState(ResourceState::FragmentShaderResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        AppendResourceState(ResourceState::IndirectArgument, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
        AppendResourceState(ResourceState::CopyDestination, D3D12_RESOURCE_STATE_COPY_DEST);
        AppendResourceState(ResourceState::CopySource, D3D12_RESOURCE_STATE_COPY_SOURCE);
        AppendResourceState(ResourceState::Present, D3D12_RESOURCE_STATE_PRESENT);

        return nativeState;
    }
}

void commands::CloseCommandBuffer(CommandBufferHandle cmdBufferHdl)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);
    pCmdBuffer->m_pCmdList->Close();
}

void commands::SetComputeShaderResourceLayout(CommandBufferHandle cmdBufferHdl, ShaderResourceLayoutHandle srlHdl)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    ID3D12RootSignature* pRootSig;
    AsType(pRootSig, srlHdl);

    pCmdBuffer->m_pCmdList->SetComputeRootSignature(pRootSig);
}

void commands::SetGraphicsShaderResourceLayout(CommandBufferHandle cmdBufferHdl, ShaderResourceLayoutHandle srlHdl)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    ID3D12RootSignature* pRootSig;
    AsType(pRootSig, srlHdl);

    pCmdBuffer->m_pCmdList->SetGraphicsRootSignature(pRootSig);
}

void commands::SetComputeConstants(CommandBufferHandle cmdBufferHdl, uint32_t index, uint32_t valueCount, const uint32_t* values)
{
    CommandBuffer* pCmdBuffer = ToType(cmdBufferHdl);
    pCmdBuffer->m_pCmdList->SetComputeRoot32BitConstants(index, valueCount, values, 0);
}

void commands::SetGraphicsConstants(CommandBufferHandle cmdBufferHdl, uint32_t index, uint32_t valueCount, const uint32_t* values)
{
    CommandBuffer* pCmdBuffer = ToType(cmdBufferHdl);
    pCmdBuffer->m_pCmdList->SetGraphicsRoot32BitConstants(index, valueCount, values, 0);
}

void commands::SetDescriptorHeaps(CommandBufferHandle cmdBufferHdl)
{
    CommandBuffer* pCmdBuffer = AsType<CommandBuffer>(cmdBufferHdl);
    ID3D12DescriptorHeap* pViewHeap = pCmdBuffer->m_pViewDescriptorHeap->m_pDescriptorHeap.Get();
    pCmdBuffer->m_pCmdList->SetDescriptorHeaps(1, &pViewHeap);
}

void commands::SetGraphicsConstantBuffer(const CommandBufferHandle cmdBufferHdl, const BufferHandle cbvHandle, const uint32_t index)
{
    CommandBuffer* pCmdBuffer = AsType<CommandBuffer>(cmdBufferHdl);
    Buffer* pBuffer = AsType<Buffer>(cbvHandle);
    pCmdBuffer->m_pCmdList->SetGraphicsRootConstantBufferView(index, pBuffer->m_pResource->GetGPUVirtualAddress());
}

void commands::SetComputeDescriptorTable(CommandBufferHandle cmdBufferHdl, uint32_t index, const DescriptorTable& table)
{

}

void commands::SetGraphicsDescriptorTable(CommandBufferHandle cmdBufferHdl, uint32_t index, const DescriptorTable& table)
{

}

void commands::SetGfxPipeline(CommandBufferHandle cmdBufferHdl, GfxPipelineHandle pipelineHdl)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    ID3D12PipelineState* pPipeline;
    AsType(pPipeline, pipelineHdl);

    pCmdBuffer->m_pCmdList->SetPipelineState(pPipeline);
}

void commands::SetComputePipeline(CommandBufferHandle cmdBufferHdl, ComputePipelineHandle pipelineHdl)
{

}

void commands::DispatchCompute(CommandBufferHandle cmdBufferHdl, uint32_t x, uint32_t y, uint32_t z)
{

}

void commands::DrawInstanced(
    CommandBufferHandle cmdBufferHdl,
    uint32_t vertexCountPerInstance,
    uint32_t instanceCount,
    uint32_t startVertex,
    uint32_t startInstance)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    pCmdBuffer->m_pCmdList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertex, startInstance);
}

void commands::DrawIndexedInstanced(
    CommandBufferHandle cmdBufferHdl,
    uint32_t indexCountPerInstance,
    uint32_t instanceCount,
    uint32_t startIndex,
    uint32_t baseVertex,
    uint32_t startInstance)
{
    CommandBuffer* const pCmdBuffer = AsType<CommandBuffer>(cmdBufferHdl);
    pCmdBuffer->m_pCmdList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndex, baseVertex, startInstance);
}


void commands::ClearDepthStencil(CommandBufferHandle cmdBufferHdl, TextureHandle depthStencilHdl)
{
    CommandBuffer* pCmdBuffer = AsType<CommandBuffer>(cmdBufferHdl);
    Texture* pDepthStencilTexture = AsType<Texture>(depthStencilHdl);

    constexpr D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
    pCmdBuffer->m_pCmdList->ClearDepthStencilView(pDepthStencilTexture->m_cbdbHandle, clearFlags, 1.f, 0, 0, nullptr);
}

void commands::ClearRenderTarget(CommandBufferHandle cmdBufferHdl, TextureHandle renderTargetHdl, Vector4 clearColor)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    Texture* pTexture;
    AsType(pTexture, renderTargetHdl);

    pCmdBuffer->m_pCmdList->ClearRenderTargetView(pTexture->m_cbdbHandle, clearColor.f, 0, nullptr);
}

void commands::SetIndexBuffer(CommandBufferHandle cmdBufferHdl, BufferHandle indexBufferHdl)
{
    CommandBuffer* const pCmdBuffer = AsType<CommandBuffer>(cmdBufferHdl);
    Buffer* const pIndexBuffer = AsType<Buffer>(indexBufferHdl);

    D3D12_INDEX_BUFFER_VIEW ibvDesc = {};
    ibvDesc.BufferLocation = pIndexBuffer->m_pResource->GetGPUVirtualAddress();
    ibvDesc.SizeInBytes = pIndexBuffer->m_byteSize;
    ibvDesc.Format = DXGI_FORMAT_R32_UINT;
    pCmdBuffer->m_pCmdList->IASetIndexBuffer(&ibvDesc);
}

void commands::SetPrimitiveTopology(CommandBufferHandle cmdBufferHdl, PrimitiveTopology topology)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    const D3D12_PRIMITIVE_TOPOLOGY nativeTopology = ToNativePrimitiveTopology(topology);
    pCmdBuffer->m_pCmdList->IASetPrimitiveTopology(nativeTopology);
}

void commands::SetVertexBuffers(
    CommandBufferHandle cmdBufferHdl,
    uint32_t startSlot,
    uint32_t bufferCount,
    const BufferHandle* pVertexBufferHdls)
{
    CommandBuffer* const pCmdBuffer = AsType<CommandBuffer>(cmdBufferHdl);

    D3D12_VERTEX_BUFFER_VIEW vbv[D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
    BIOME_ASSERT(bufferCount <= D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);

    for (uint32_t i = 0; i < bufferCount; ++i)
    {
        const uint32_t slotIndex = startSlot + i;
        Buffer* const pVertexBuffer = AsType<Buffer>(pVertexBufferHdls[i]);

        vbv[i].BufferLocation = pVertexBuffer->m_pResource->GetGPUVirtualAddress();
        vbv[i].SizeInBytes = pVertexBuffer->m_byteSize;
        vbv[i].StrideInBytes = pVertexBuffer->m_stride;
    }

    pCmdBuffer->m_pCmdList->IASetVertexBuffers(startSlot, bufferCount, vbv);
}

void commands::OMSetBlendFactor(CommandBufferHandle cmdBufferHdl, const float* pBlendfactors)
{

}

void commands::OMSetStencilRef(CommandBufferHandle cmdBufferHdl, uint32_t stencilRef)
{

}

void commands::OMSetRenderTargets(
    CommandBufferHandle cmdBufferHdl,
    uint32_t rtCount,
    const TextureHandle* pRenderTargets,
    const TextureHandle* pDepthStencil)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    constexpr uint32_t MaxRtCount = 8;
    BIOME_ASSERT(rtCount <= MaxRtCount);
    D3D12_CPU_DESCRIPTOR_HANDLE rtDescriptors[MaxRtCount] = {};

    for (uint32_t i = 0; i < rtCount; ++i)
    {
        Texture* pRt = AsType<Texture>(pRenderTargets[i]);
        rtDescriptors[i] = pRt->m_cbdbHandle;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE* pDsDescriptor = nullptr;
    if (pDepthStencil != nullptr)
    {
        Texture* pDs = AsType<Texture>(*pDepthStencil);
        pDsDescriptor = &pDs->m_cbdbHandle;
    }

    pCmdBuffer->m_pCmdList->OMSetRenderTargets(rtCount, rtDescriptors, FALSE, pDsDescriptor);
}

void commands::RSSetScissorRects(CommandBufferHandle cmdBufferHdl, uint32_t count, const Rectangle* pRectangles)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    D3D12_RECT rects[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    for (uint32_t i = 0; i < count; ++i)
    {
        D3D12_RECT& nativeRect = rects[i];
        const Rectangle& biomeRect = pRectangles[i];

        nativeRect.left = biomeRect.m_left;
        nativeRect.right = biomeRect.m_right;
        nativeRect.top = biomeRect.m_top;
        nativeRect.bottom = biomeRect.m_bottom;
    }

    pCmdBuffer->m_pCmdList->RSSetScissorRects(count, rects);
}

void commands::RSSetViewports(CommandBufferHandle cmdBufferHdl, uint32_t count, const Viewport* pViewports)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    D3D12_VIEWPORT viewports[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    for (uint32_t i = 0; i < count; ++i)
    {
        D3D12_VIEWPORT& nativeVp = viewports[i];
        const Viewport& biomeVp = pViewports[i];

        nativeVp.Width = biomeVp.m_width;
        nativeVp.Height = biomeVp.m_height;
        nativeVp.TopLeftX = biomeVp.m_x;
        nativeVp.TopLeftY = biomeVp.m_y;
        nativeVp.MinDepth = biomeVp.m_minDepth;
        nativeVp.MaxDepth = biomeVp.m_maxDepth;
    }

    pCmdBuffer->m_pCmdList->RSSetViewports(count, viewports);
}

void commands::ResourceTransition(CommandBufferHandle cmdBufferHdl, const TextureStateTransition* transitions, uint32_t transitionCount)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    D3D12_RESOURCE_BARRIER* pBarriers = reinterpret_cast<D3D12_RESOURCE_BARRIER*>(memory::StackAlloc(transitionCount * sizeof(D3D12_RESOURCE_BARRIER)));

    for (uint32_t i = 0; i < transitionCount; ++i)
    {
        const TextureStateTransition& transition = transitions[i];

        Texture* pTexture;
        AsType(pTexture, transition.m_textureHdl);

        D3D12_RESOURCE_BARRIER& barrier = pBarriers[i];
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = pTexture->m_pResource.Get();
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = ToNativeResourceState(transition.m_before);
        barrier.Transition.StateAfter = ToNativeResourceState(transition.m_after);
    }

    pCmdBuffer->m_pCmdList->ResourceBarrier(transitionCount, pBarriers);
}

void commands::CopyBuffer(
    const CommandBufferHandle cmdBufferHdl,
    const BufferHandle srcHdl,
    const BufferHandle dstHdl,
    const uint32_t srcOffset,
    const uint32_t dstOffset,
    const uint32_t byteSize)
{
    CommandBuffer* pCmdBuffer = AsType<CommandBuffer>(cmdBufferHdl);
    Buffer* pSrcBuffer = AsType<Buffer>(srcHdl);
    Buffer* pDstBuffer = AsType<Buffer>(dstHdl);
    pCmdBuffer->m_pCmdList->CopyBufferRegion(pDstBuffer->m_pResource.Get(), dstOffset, pSrcBuffer->m_pResource.Get(), srcOffset, byteSize);
}
