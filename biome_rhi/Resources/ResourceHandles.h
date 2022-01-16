#pragma once

#include <stdint.h>
#include <utility>
#include <limits>
#include "biome_core/Core/Defines.h"
#include "biome_core/Math/Math.h"
#include "biome_core/Handle/Handle.h"
#include "biome_core/DataStructures/StaticArray.h"

namespace biome
{
    namespace rhi
    {
        struct alignas(sizeof(uintptr_t)) BasicHandle
        {
            uintptr_t m_handle { Handle_NULL };

            BasicHandle() = default;
            BasicHandle(uintptr_t hdl) : m_handle(hdl) {};
        };

        static_assert(sizeof(BasicHandle) == sizeof(uintptr_t));

        #define DefineHandle(name) \
            struct name : BasicHandle { name() = default; name(uintptr_t hdl) : BasicHandle(hdl){} }

        typedef uintptr_t TextureHandle;
        typedef uintptr_t BufferHandle;

        typedef Handle DescriptorHeapHandle;
        typedef Handle RenderPassHandle;

        typedef uintptr_t WindowHandle;
        typedef uintptr_t AppHandle;
        typedef uintptr_t LibraryHandle;

        DefineHandle(GpuDeviceHandle);
        DefineHandle(CommandQueueHandle);
        DefineHandle(CommandBufferHandle);
        DefineHandle(FenceHandle);
        DefineHandle(SwapChainHandle);
        DefineHandle(ShaderResourceLayoutHandle);
        DefineHandle(GfxPipelineHandle);
        DefineHandle(ComputePipelineHandle);

        typedef biome::data::StaticArray<uint8_t> ShaderHandle;
    }
}