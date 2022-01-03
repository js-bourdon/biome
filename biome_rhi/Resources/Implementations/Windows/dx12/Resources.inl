#pragma once

namespace biome::rhi
{
    namespace resources
    {
        struct GpuDevice
        {
			ID3D12Device*           m_pDevice { nullptr };
            ID3D12DescriptorHeap*   m_pRtvDescriptorHeap { nullptr };
			uint64_t                m_currentFrame { 0 };
            uint32_t                m_rtvDescriptorSize { 0 };
            uint32_t                m_framesOfLatency { 0 };
        };

        struct SwapChain
        {
			IDXGISwapChain1*                        m_pSwapChain { nullptr };
            biome::data::StaticArray<TextureHandle> m_backBuffers {};
            uint32_t                                m_pixelWidth { 0 };
            uint32_t                                m_pixelHeight { 0 };
        };

        struct CommandBuffer
        {
            biome::data::StaticArray<ID3D12CommandAllocator*>   m_cmdAllocators {};
            ID3D12GraphicsCommandList*                          m_pCmdList { nullptr };
        };

        struct Texture
        {
            ID3D12Resource* m_pResource { nullptr };
            D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle {};
        };
    }
}