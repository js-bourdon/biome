#pragma once

#include "biome_rhi/Resources/ResourceHandles.h"
#include "biome_rhi/Systems/SystemEnums.h"

namespace biome::rhi::descriptors
{
    struct ShaderResourceLayoutDesc;
    struct GfxPipelineDesc;
    struct ComputePipelineDesc;
    struct RayTracingInstanceDesc;
    enum class Format;
}

using namespace biome::rhi::descriptors;

namespace biome::rhi
{
    enum class CommandType;
    enum class BufferType;

    namespace device
    {
        GpuDeviceHandle             CreateDevice(const uint32_t framesOfLatency, const bool useCpuEmulation);
        CommandBufferHandle         CreateCommandBuffer(GpuDeviceHandle deviceHdl, CommandType type);

        SwapChainHandle             CreateSwapChain(
                                        GpuDeviceHandle deviceHdl,
                                        WindowHandle windowHdl,
                                        uint32_t pixelWidth,
                                        uint32_t pixelHeight);

        void                        StartFrame(GpuDeviceHandle deviceHdl);
        void                        EndFrame(GpuDeviceHandle deviceHdl);
        void                        OnResourceUpdatesDone(GpuDeviceHandle deviceHdl);
        void                        DrainPipeline(GpuDeviceHandle deviceHdl);

        uint64_t                    GetCurrentFrameIndex(GpuDeviceHandle deviceHdl);
        Format                      GetSwapChainFormat(SwapChainHandle hdl);
        TextureHandle               GetBackBuffer(GpuDeviceHandle deviceHdl, SwapChainHandle hdl);
        bool                        IsRaytracingSupported(GpuDeviceHandle deviceHdl);

        ShaderHandle                CreateShader(GpuDeviceHandle deviceHdl, const char* pFilePath);
        ShaderResourceLayoutHandle  CreateShaderResourceLayout(GpuDeviceHandle deviceHdl, const ShaderResourceLayoutDesc& desc);
        ShaderResourceLayoutHandle  CreateShaderResourceLayout(GpuDeviceHandle deviceHdl, const char* pFilePath);
        GfxPipelineHandle           CreateGraphicsPipeline(GpuDeviceHandle deviceHdl, const GfxPipelineDesc& desc);
        ComputePipelineHandle       CreateComputePipeline(GpuDeviceHandle deviceHdl, const ComputePipelineDesc& desc);
        DescriptorHeapHandle        CreateDescriptorHeap(GpuDeviceHandle deviceHdl);

        BufferHandle                CreateBuffer(
                                        const GpuDeviceHandle deviceHdl, 
                                        const BufferType type, 
                                        const uint32_t bufferByteSize, 
                                        const uint32_t stride, 
                                        const Format format);

        TextureHandle               CreateTexture(
                                        const GpuDeviceHandle deviceHdl,
                                        const uint32_t pixelWidth,
                                        const uint32_t pixelHeight,
                                        const descriptors::Format format,
                                        const bool allowRtv,
                                        const bool allowDsv,
                                        const bool allowUav);

        AccelerationStructureHandle CreateRtAccelerationStructure(
                                        const RayTracingInstanceDesc* const pRtInstances, 
                                        const uint32_t instanceCount);

        void*                       MapBuffer(GpuDeviceHandle deviceHdl, BufferHandle hdl);
        void                        UnmapBuffer(GpuDeviceHandle deviceHdl, BufferHandle hdl);

        void                        DestroyDevice(GpuDeviceHandle hdl);
        void                        DestroyCommandQueue(CommandQueueHandle hdl);
        void                        DestroySwapChain(SwapChainHandle hdl);
        void                        DestroyShaderResourceLayout(ShaderResourceLayoutHandle hdl);
        void                        DestroyGfxPipeline(GfxPipelineHandle hdl);
        void                        DestroyComputePipeline(ComputePipelineHandle hdl);
        void                        DestroyDescriptorHeap(DescriptorHeapHandle hdl);
        void                        DestroyBuffer(GpuDeviceHandle deviceHdl, BufferHandle bufferHdl);
        void                        DestroyTexture(GpuDeviceHandle deviceHdl, TextureHandle textureHdl);

        void                        SignalFence(FenceHandle fenceHdl);
        void                        GPUWaitOnFence(FenceHandle fenceHdl);
        void                        CPUWaitOnFence(FenceHandle fenceHdl);
        void                        ExecuteCommandBuffer(GpuDeviceHandle deviceHdl, CommandBufferHandle cmdBufferHdl);
        void                        Present(SwapChainHandle swapChainHdl);
    }

}