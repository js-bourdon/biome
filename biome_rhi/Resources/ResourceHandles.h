#pragma once

#include <stdint.h>
#include <utility>
#include <limits>
#include "biome_core/Core/Defines.h"
#include "biome_core/Math/Math.h"
#include "biome_core/Handle/Handle.h"

namespace biome
{
    namespace rhi
    {
        using TextureHandle = ParentResourceHandle<0x1F>;
        using Texture2DHandle = ChildResourceHandle<TextureHandle, 0x01>;
        using Texture3DHandle = ChildResourceHandle<TextureHandle, 0x02>;
        using TextureCubeHandle = ChildResourceHandle<TextureHandle, 0x04>;
        using TextureArrayHandle = ChildResourceHandle<TextureHandle, 0x08>;
        using TextureSubResource = ChildResourceHandle<TextureHandle, 0x10>;

        using BufferHandle = ParentResourceHandle<0x1F>;
        using ConstantBufferHandle = ChildResourceHandle<BufferHandle, 0x01>;
        using RWBufferHandle = ChildResourceHandle<BufferHandle, 0x02>;
        using StructuredBufferHandle = ChildResourceHandle<BufferHandle, 0x04>;
        using VertexBufferHandle = ChildResourceHandle<BufferHandle, 0x08>;
        using IndexBufferHandle = ChildResourceHandle<BufferHandle, 0x10>;

        typedef Handle DescriptorHeapHandle;
        typedef Handle RenderPassHandle;

        typedef uintptr_t WindowHandle;
        typedef uintptr_t AppHandle;
        typedef uintptr_t LibraryHandle;

        typedef uintptr_t GpuDeviceHandle;
        typedef uintptr_t CommandQueueHandle;
        typedef uintptr_t CommandListHandle;
        typedef uintptr_t FenceHandle;
        typedef uintptr_t SwapChainHandle;
        typedef uintptr_t CommandAllocatorHandle;
        typedef uintptr_t ShaderResourceLayoutHandle;
        typedef uintptr_t GfxPipelineHandle;
        typedef uintptr_t ComputePipelineHandle;
    }
}