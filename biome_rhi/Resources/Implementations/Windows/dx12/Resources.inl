#pragma once

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
    }
}