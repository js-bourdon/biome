#pragma once

#include "biome_rhi/Systems/CommandSystem.h"
#include "biome_rhi/Resources/Resources.h"
#include "biome_core/Core/Globals.h"

using namespace biome;
using namespace biome::rhi;
using namespace biome::rhi::resources;

namespace
{
    static D3D12_RESOURCE_STATES ToNativeResourceState(commands::ResourceState rscState)
    {
        #define AppendResourceState(biomeName, nativeName) \
            nativeState = core::CombineFlags(nativeState, core::HasFlag(rscState, biomeName) ? nativeName : D3D12_RESOURCE_STATE_COMMON)

        D3D12_RESOURCE_STATES nativeState = D3D12_RESOURCE_STATE_COMMON;

        AppendResourceState(commands::ResourceState::VertexBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        AppendResourceState(commands::ResourceState::ConstantBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        AppendResourceState(commands::ResourceState::IndexBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);
        AppendResourceState(commands::ResourceState::RenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET);
        AppendResourceState(commands::ResourceState::UnorderedAccess, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        AppendResourceState(commands::ResourceState::DepthWrite, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        AppendResourceState(commands::ResourceState::DepthRead, D3D12_RESOURCE_STATE_DEPTH_READ);
        AppendResourceState(commands::ResourceState::GeometryShaderResource, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        AppendResourceState(commands::ResourceState::FragmentShaderResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        AppendResourceState(commands::ResourceState::IndirectArgument, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
        AppendResourceState(commands::ResourceState::CopyDestination, D3D12_RESOURCE_STATE_COPY_DEST);
        AppendResourceState(commands::ResourceState::CopySource, D3D12_RESOURCE_STATE_COPY_SOURCE);
        AppendResourceState(commands::ResourceState::Present, D3D12_RESOURCE_STATE_PRESENT);

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

}

void commands::SetGraphicsShaderResourceLayout(CommandBufferHandle cmdBufferHdl, ShaderResourceLayoutHandle srlHdl)
{

}

void commands::SetComputeConstant(CommandBufferHandle cmdBufferHdl, uint32_t index, uint32_t value, uint32_t destOffsetInValues)
{

}

void commands::SetDescriptorHeaps(CommandBufferHandle cmdBufferHdl, uint32_t count, const DescriptorHeapHandle* pHeapHdls)
{

}

void commands::SetComputeDescriptorTable(CommandBufferHandle cmdBufferHdl, uint32_t index, const DescriptorTable& table)
{

}

void commands::SetGraphicsDescriptorTable(CommandBufferHandle cmdBufferHdl, uint32_t index, const DescriptorTable& table)
{

}

void commands::SetGfxPipeline(CommandBufferHandle cmdBufferHdl, GfxPipelineHandle pipelineHdl)
{

}

void commands::SetComputePipeline(CommandBufferHandle cmdBufferHdl, ComputePipelineHandle pipelineHdl)
{

}

void commands::DispatchCompute(CommandBufferHandle cmdBufferHdl, uint32_t x, uint32_t y, uint32_t z)
{

}

void commands::DrawIndexedInstanced(
    CommandBufferHandle cmdBufferHdl,
    uint32_t vertexCountPerInstance,
    uint32_t instanceCount,
    uint32_t startVertex,
    uint32_t startInstance)
{

}

void commands::DrawInstanced(
    CommandBufferHandle cmdBufferHdl,
    uint32_t indexCountPerInstance,
    uint32_t instanceCount,
    uint32_t startIndex,
    uint32_t baseVertex,
    uint32_t startInstance)
{

}


void commands::ClearDepthStencil(CommandBufferHandle cmdBufferHdl, TextureHandle depthStencilHdl)
{

}

void commands::ClearRenderTarget(CommandBufferHandle cmdBufferHdl, TextureHandle renderTargetHdl, Vector4 clearColor)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    Texture* pTexture;
    AsType(pTexture, renderTargetHdl);

    pCmdBuffer->m_pCmdList->ClearRenderTargetView(pTexture->m_rtvHandle, clearColor.f, 0, nullptr);
}

void commands::SetIndexBuffer(CommandBufferHandle cmdBufferHdl, BufferHandle indexBufferHdl)
{

}

void commands::SetPrimitiveTopology(CommandBufferHandle cmdBufferHdl, PrimitiveTopology topology)
{

}

void commands::SetVertexBuffers(
    CommandBufferHandle cmdBufferHdl,
    uint32_t startSlot,
    uint32_t bufferCount,
    const BufferHandle* pVertexBufferHdls)
{

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

}

void commands::RSSetScissorRects(CommandBufferHandle cmdBufferHdl, uint32_t count, const Rectangle* pRectangles)
{

}

void commands::RSSetViewports(CommandBufferHandle cmdBufferHdl, uint32_t count, const Viewport* pViewports)
{

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
