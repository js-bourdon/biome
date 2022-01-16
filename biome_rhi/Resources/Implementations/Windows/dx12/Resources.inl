#pragma once

#include "biome_core/DataStructures/StaticArray.h"

namespace biome::rhi
{
    namespace resources
    {
        struct GpuDevice
        {
			ComPtr<ID3D12Device>            m_pDevice { nullptr };
            ComPtr<ID3D12DescriptorHeap>    m_pRtvDescriptorHeap { nullptr };
            ComPtr<ID3D12Fence>             m_pFrameFence { nullptr };
			uint64_t                        m_currentFrame { 0 };
            HANDLE                          m_fenceEvent {};
            uint32_t                        m_rtvDescriptorSize { 0 };
            uint32_t                        m_framesOfLatency { 0 };
        };

        struct SwapChain
        {
			ComPtr<IDXGISwapChain1>                 m_pSwapChain { nullptr };
            biome::data::StaticArray<TextureHandle> m_backBuffers {};
            uint32_t                                m_pixelWidth { 0 };
            uint32_t                                m_pixelHeight { 0 };
        };

        struct CommandBuffer
        {
            typedef biome::data::StaticArray<ComPtr<ID3D12CommandAllocator>, CleanConstructDestruct> AllocatorArray;

            AllocatorArray                      m_cmdAllocators {};
            ComPtr<ID3D12GraphicsCommandList>   m_pCmdList { nullptr };
        };

        struct Texture
        {
            ComPtr<ID3D12Resource>      m_pResource { nullptr };
            D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle {};
        };

        struct Buffer
        {
            ComPtr<ID3D12Resource>      m_pResource { nullptr };
            ComPtr<ID3D12Resource>      m_pStaging { nullptr };
            D3D12_CPU_DESCRIPTOR_HANDLE m_srv {};
            D3D12_CPU_DESCRIPTOR_HANDLE m_uav {};
            size_t                      m_byteSize { 0 };
        };

        struct DescriptorHeap
        {

        };
    }

    using namespace biome::rhi::resources;

    template<typename PtrType, typename HandleType>
    inline void AsType(PtrType& pPtr, HandleType handle)
    {
        pPtr = reinterpret_cast<PtrType>(*reinterpret_cast<uintptr_t*>(&handle));
    }

    template<typename PtrType, typename HandleType>
    inline void AsHandle(const PtrType pPtr, HandleType& handle)
    {
        uintptr_t* pHdl = reinterpret_cast<uintptr_t*>(&handle);
        *pHdl = reinterpret_cast<uintptr_t>(pPtr);
    }

    inline GpuDevice* ToType(GpuDeviceHandle hdl)
    {
        GpuDevice* pDevice;
        AsType(pDevice, hdl);
        return pDevice;
    }

    inline GpuDeviceHandle ToHandle(GpuDevice* pDevice)
    {
        GpuDeviceHandle hdl;
        AsHandle(pDevice, hdl);
        return hdl;
    }

    inline CommandBuffer* ToType(CommandBufferHandle hdl)
    {
        CommandBuffer* pCmdBuffer;
        AsType(pCmdBuffer, hdl);
        return pCmdBuffer;
    }

    inline CommandBufferHandle ToHandle(CommandBuffer* pCmdBuffer)
    {
        CommandBufferHandle hdl;
        AsHandle(pCmdBuffer, hdl);
        return hdl;
    }

    inline Buffer* ToType(BufferHandle hdl)
    {
        Buffer* pBuffer;
        AsType(pBuffer, hdl);
        return pBuffer;
    }

    inline BufferHandle ToHandle(Buffer* pBuffer)
    {
        BufferHandle hdl;
        AsHandle(pBuffer, hdl);
        return hdl;
    }

    inline DescriptorHeap* ToType(DescriptorHeapHandle hdl)
    {
        DescriptorHeap* pHeap;
        AsType(pHeap, hdl);
        return pHeap;
    }

    inline DescriptorHeapHandle ToHandle(DescriptorHeap* pHeap)
    {
        DescriptorHeapHandle hdl;
        AsHandle(pHeap, hdl);
        return hdl;
    }
}