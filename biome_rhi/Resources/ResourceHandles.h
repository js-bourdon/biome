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
        template<typename PtrType, typename HandleType>
        inline void AsType(PtrType& pPtr, HandleType handle)
        {
            pPtr = reinterpret_cast<PtrType>(handle);
        }

        template<typename PtrType, typename HandleType>
        inline void AsHandle(const PtrType pPtr, HandleType& handle)
        {
            handle = reinterpret_cast<HandleType>(pPtr);
        }

        typedef uintptr_t TextureHandle;
        typedef uintptr_t BufferHandle;

        typedef Handle DescriptorHeapHandle;
        typedef Handle RenderPassHandle;

        typedef uintptr_t WindowHandle;
        typedef uintptr_t AppHandle;
        typedef uintptr_t LibraryHandle;

        typedef uintptr_t GpuDeviceHandle;
        typedef uintptr_t CommandQueueHandle;
        typedef uintptr_t CommandBufferHandle;
        typedef uintptr_t FenceHandle;
        typedef uintptr_t SwapChainHandle;
        typedef uintptr_t ShaderResourceLayoutHandle;
        typedef uintptr_t GfxPipelineHandle;
        typedef uintptr_t ComputePipelineHandle;

        typedef biome::data::StaticArray<uint8_t> ShaderHandle;
    }
}