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

        inline bool operator==(uintptr_t value, const BasicHandle& hdl)
        {
            return value == hdl.m_handle;
        }

        inline bool operator!=(uintptr_t value, const BasicHandle& hdl)
        {
            return value != hdl.m_handle;
        }

        inline bool operator==(const BasicHandle& hdl, uintptr_t value)
        {
            return hdl.m_handle == value;
        }

        inline bool operator!=(const BasicHandle& hdl, uintptr_t value)
        {
            return hdl.m_handle != value;
        }

        static_assert(sizeof(BasicHandle) == sizeof(uintptr_t));

        #define DefineHandle(name) \
            struct name : BasicHandle { name() = default; name(uintptr_t hdl) : BasicHandle(hdl){} }

        DefineHandle(TextureHandle);
        DefineHandle(BufferHandle);
        DefineHandle(GpuDeviceHandle);
        DefineHandle(CommandQueueHandle);
        DefineHandle(CommandBufferHandle);
        DefineHandle(FenceHandle);
        DefineHandle(SwapChainHandle);
        DefineHandle(ShaderResourceLayoutHandle);
        DefineHandle(GfxPipelineHandle);
        DefineHandle(ComputePipelineHandle);
        DefineHandle(DescriptorHeapHandle);

        typedef uintptr_t WindowHandle;
        typedef uintptr_t AppHandle;
        typedef uintptr_t LibraryHandle;

        typedef biome::data::StaticArray<uint8_t> ShaderHandle;
    }
}