#pragma once

#include "biome_rhi/Resources/ResourceHandles.h"
#include "biome_rhi/Systems/SystemEnums.h"

namespace biome::rhi::descriptors
{
    struct ShaderResourceLayoutDesc;
    struct GfxPipelineDesc;
    struct ComputePipelineDesc;
    enum class PixelFormat;
}

using namespace biome::rhi::descriptors;

namespace biome::rhi
{
    enum class CommandType;

    namespace device
    {
        GpuDeviceHandle             CreateDevice(uint32_t framesOfLatency);
        CommandQueueHandle          CreateCommandQueue(GpuDeviceHandle deviceHdl, CommandType type);
        CommandBufferHandle         CreateCommandBuffer(GpuDeviceHandle deviceHdl, CommandType type);

        SwapChainHandle             CreateSwapChain(
                                        GpuDeviceHandle deviceHdl,
                                        CommandQueueHandle cmdQueueHdl,
                                        WindowHandle windowHdl,
                                        uint32_t pixelWidth,
                                        uint32_t pixelHeight);

        void                        StartFrame(GpuDeviceHandle deviceHdl, CommandQueueHandle cmdQueueHdl, CommandBufferHandle cmdBufferHdl);
        void                        EndFrame(GpuDeviceHandle deviceHdl, CommandQueueHandle cmdQueueHdl, CommandBufferHandle cmdBufferHdl);

        uint64_t                    GetCurrentFrameIndex(GpuDeviceHandle deviceHdl);
        PixelFormat                 GetSwapChainFormat(SwapChainHandle hdl);
        TextureHandle               GetBackBuffer(GpuDeviceHandle deviceHdl, SwapChainHandle hdl);

        ShaderHandle                CreateShader(GpuDeviceHandle deviceHdl, const char* pFilePath);
        ShaderResourceLayoutHandle  CreateShaderResourceLayout(GpuDeviceHandle deviceHdl, const ShaderResourceLayoutDesc& desc);
        ShaderResourceLayoutHandle  CreateShaderResourceLayout(GpuDeviceHandle deviceHdl, const char* pFilePath);
        GfxPipelineHandle           CreateGraphicsPipeline(GpuDeviceHandle deviceHdl, const GfxPipelineDesc& desc);
        ComputePipelineHandle       CreateComputePipeline(GpuDeviceHandle deviceHdl, const ComputePipelineDesc& desc);
        DescriptorHeapHandle        CreateDescriptorHeap(GpuDeviceHandle deviceHdl);

        void                        DestroyDevice(GpuDeviceHandle hdl);
        void                        DestroyCommandQueue(CommandQueueHandle hdl);
        void                        DestroyCommandBuffer(CommandBufferHandle cmdBufferHdl);
        void                        DestroySwapChain(SwapChainHandle hdl);
        void                        DestroyShaderResourceLayout(ShaderResourceLayoutHandle hdl);
        void                        DestroyGfxPipeline(GfxPipelineHandle hdl);
        void                        DestroyComputePipeline(ComputePipelineHandle hdl);
        void                        DestroyDescriptorHeap(DescriptorHeapHandle hdl);

        void                        SignalFence(FenceHandle fenceHdl);
        void                        GPUWaitOnFence(FenceHandle fenceHdl);
        void                        CPUWaitOnFence(FenceHandle fenceHdl);
        void                        ExecuteCommandBuffer(CommandQueueHandle cmdQueueHdl, CommandBufferHandle cmdBufferHdl);
        void                        Present(SwapChainHandle swapChainHdl);
    }

}