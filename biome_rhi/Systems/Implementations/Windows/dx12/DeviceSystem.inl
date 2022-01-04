#pragma once

#include "biome_rhi/Systems/DeviceSystem.h"
#include "biome_rhi/Dependencies/d3d12.h"
#include "biome_rhi/Descriptors/ShaderResourceLayoutDesc.h"
#include "biome_rhi/Descriptors/PipelineDesc.h"
#include "biome_rhi/Resources/Resources.h"
#include "biome_core/DataStructures/Vector.h"
#include "biome_core/Memory/StackAllocator.h"
#include "biome_core/FileSystem/FileSystem.h"

using namespace biome;
using namespace biome::rhi;
using namespace biome::rhi::resources;

namespace 
{
    static bool GetHardwareAdapter(
        IDXGIFactory1* pFactory,
        IDXGIAdapter1** ppAdapter)
    {
        *ppAdapter = nullptr;

        ComPtr<IDXGIAdapter1> adapter;
        ComPtr<IDXGIFactory6> factory6;

        if(SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            for(
                UINT adapterIndex = 0;
                DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
                    adapterIndex,
                    DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                    IID_PPV_ARGS(&adapter));
                ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if(SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
                {
                    *ppAdapter = adapter.Detach();
                    return true;
                }
            }
        }
        else
        {
            for(UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    // Don't select the Basic Render Driver adapter.
                    // If you want a software adapter, pass in "/warp" on the command line.
                    continue;
                }

                // Check to see whether the adapter supports Direct3D 12, but don't create the
                // actual device yet.
                if(SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
                {
                    *ppAdapter = adapter.Detach();
                    return true;
                }
            }
        }

        return false;
    }

    static D3D12_COMMAND_LIST_TYPE ToNativeCmdType(CommandType type)
    {
        D3D12_COMMAND_LIST_TYPE cmdType;
        switch(type)
        {
        case CommandType::Graphics:
            cmdType = D3D12_COMMAND_LIST_TYPE_DIRECT;
            break;
        case CommandType::AsyncCompute:
            cmdType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            break;
        case CommandType::Copy:
            cmdType = D3D12_COMMAND_LIST_TYPE_COPY;
            break;
        default:
            cmdType = D3D12_COMMAND_LIST_TYPE_DIRECT;
            BIOME_ASSERT_MSG(false, "Invalid CommandType");
        }

        return cmdType;
    }

    static ID3D12CommandAllocator* CreateCommandAllocator(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE cmdType)
    {
        ID3D12CommandAllocator* pCmdAlloc;
        BIOME_ASSERT_ALWAYS_EXEC(SUCCEEDED(pDevice->CreateCommandAllocator(cmdType, IID_PPV_ARGS(&pCmdAlloc))));
        return pCmdAlloc;
    }

    static ID3D12GraphicsCommandList* CreateCommandList(ID3D12Device* pDevice, ID3D12CommandAllocator* pCmdAllocator, D3D12_COMMAND_LIST_TYPE cmdType)
    {
        ID3D12GraphicsCommandList* pCmdList = nullptr;
        BIOME_ASSERT_ALWAYS_EXEC(SUCCEEDED(pDevice->CreateCommandList(0, cmdType, pCmdAllocator, nullptr, IID_PPV_ARGS(&pCmdList))));
        return pCmdList;
    }

    static D3D12_TEXTURE_ADDRESS_MODE ToNativeAddressMode(SamplerAddressMode mode)
    {
        switch(mode)
        {
        case SamplerAddressMode::Wrap:
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case SamplerAddressMode::Clamp:
            return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        default:
            BIOME_FAIL();
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        }
    }

    static D3D12_FILTER ToNativeFilter(SamplerFiltering filter)
    {
        switch(filter)
        {
        case SamplerFiltering::Point:
            return D3D12_FILTER_MIN_MAG_MIP_POINT;
        case SamplerFiltering::Linear:
            return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        case SamplerFiltering::Anisotropic:
            return D3D12_FILTER_ANISOTROPIC;
        default:
            BIOME_FAIL();
            return D3D12_FILTER_MIN_MAG_MIP_POINT;
        }
    }

    static D3D12_BLEND_OP ToNativeBlendOperation(BlendOperation blendOp)
    {
        switch(blendOp)
        {
        case BlendOperation::ADD:
            return D3D12_BLEND_OP_ADD;
        case BlendOperation::SUBTRACT:
            return D3D12_BLEND_OP_SUBTRACT;
        case BlendOperation::REV_SUBTRACT:
            return D3D12_BLEND_OP_REV_SUBTRACT;
        case BlendOperation::MIN:
            return D3D12_BLEND_OP_MIN;
        case BlendOperation::MAX:
            return D3D12_BLEND_OP_MAX;
        default:
            BIOME_FAIL();
            return D3D12_BLEND_OP_ADD;
        }
    }

    static D3D12_BLEND ToNativeBlend(BlendValue value)
    {
        switch(value)
        {
        case BlendValue::ZERO:
            return D3D12_BLEND_ZERO;
        case BlendValue::ONE:
            return D3D12_BLEND_ONE;
        case BlendValue::SRC_COLOR:
            return D3D12_BLEND_SRC_COLOR;
        case BlendValue::INV_SRC_COLOR:
            return D3D12_BLEND_INV_SRC_COLOR;
        case BlendValue::SRC_ALPHA:
            return D3D12_BLEND_SRC_ALPHA;
        case BlendValue::INV_SRC_ALPHA:
            return D3D12_BLEND_INV_SRC_ALPHA;
        case BlendValue::DEST_ALPHA:
            return D3D12_BLEND_DEST_ALPHA;
        case BlendValue::INV_DEST_ALPHA:
            return D3D12_BLEND_INV_DEST_ALPHA;
        case BlendValue::DEST_COLOR:
            return D3D12_BLEND_DEST_COLOR;
        case BlendValue::INV_DEST_COLOR:
            return D3D12_BLEND_INV_DEST_COLOR;
        default:
            BIOME_FAIL();
            return D3D12_BLEND_ZERO;
        }
    }

    static D3D12_COLOR_WRITE_ENABLE ToNativeWriteMask(bool colorWrite, bool alphaWrite)
    {
        D3D12_COLOR_WRITE_ENABLE writeMask = static_cast<D3D12_COLOR_WRITE_ENABLE>(colorWrite ?
            (D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN | D3D12_COLOR_WRITE_ENABLE_BLUE) : 0);
        writeMask = static_cast<D3D12_COLOR_WRITE_ENABLE>(writeMask | (alphaWrite ? D3D12_COLOR_WRITE_ENABLE_ALPHA : 0));

        return writeMask;
    }

    static D3D12_CULL_MODE ToNativeCullMode(CullMode cullMode)
    {
        switch(cullMode)
        {
        case CullMode::None:
            return D3D12_CULL_MODE_NONE;
        case CullMode::Front:
            return D3D12_CULL_MODE_FRONT;
        case CullMode::Back:
            return D3D12_CULL_MODE_BACK;
        default:
            BIOME_FAIL();
            return D3D12_CULL_MODE_NONE;
        }
    }

    static D3D12_COMPARISON_FUNC ToNativeComparisonFunc(ComparisonFunction func)
    {
        switch(func)
        {
        case ComparisonFunction::NEVER:
            return D3D12_COMPARISON_FUNC_NEVER;
        case ComparisonFunction::LESS:
            return D3D12_COMPARISON_FUNC_LESS;
        case ComparisonFunction::EQUAL:
            return D3D12_COMPARISON_FUNC_EQUAL;
        case ComparisonFunction::LESS_EQUAL:
            return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case ComparisonFunction::GREATER:
            return D3D12_COMPARISON_FUNC_GREATER;
        case ComparisonFunction::NOT_EQUAL:
            return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case ComparisonFunction::GREATER_EQUAL:
            return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case ComparisonFunction::ALWAYS:
            return D3D12_COMPARISON_FUNC_ALWAYS;
        default:
            BIOME_FAIL();
            return D3D12_COMPARISON_FUNC_ALWAYS;
        }
    }

    static D3D12_STENCIL_OP ToNativeStencilOp(StencilOperation stencilOp)
    {
        switch(stencilOp)
        {
        case StencilOperation::KEEP:
            return D3D12_STENCIL_OP_KEEP;
        case StencilOperation::ZERO:
            return D3D12_STENCIL_OP_ZERO;
        case StencilOperation::REPLACE:
            return D3D12_STENCIL_OP_REPLACE;
        case StencilOperation::INCR_SAT:
            return D3D12_STENCIL_OP_INCR_SAT;
        case StencilOperation::DECR_SAT:
            return D3D12_STENCIL_OP_DECR_SAT;
        case StencilOperation::INVERT:
            return D3D12_STENCIL_OP_INVERT;
        case StencilOperation::INCR:
            return D3D12_STENCIL_OP_INCR;
        case StencilOperation::DECR:
            return D3D12_STENCIL_OP_DECR;
        default:
            BIOME_FAIL();
            return D3D12_STENCIL_OP_KEEP;
        }
    }

    static DXGI_FORMAT ToNativeFormat(PixelFormat format)
    {
        switch(format)
        {
        case PixelFormat::Unknown:
            return DXGI_FORMAT_UNKNOWN;
        case PixelFormat::R32G32B32A32_FLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case PixelFormat::R32G32B32A32_UINT:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case PixelFormat::R32G32B32A32_SINT:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case PixelFormat::R32G32B32_FLOAT:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case PixelFormat::R32G32B32_UINT:
            return DXGI_FORMAT_R32G32B32_UINT;
        case PixelFormat::R32G32B32_SINT:
            return DXGI_FORMAT_R32G32B32_SINT;
        case PixelFormat::R16G16B16A16_FLOAT:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case PixelFormat::R16G16B16A16_UNORM:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case PixelFormat::R16G16B16A16_UINT:
            return DXGI_FORMAT_R16G16B16A16_UINT;
        case PixelFormat::R16G16B16A16_SNORM:
            return DXGI_FORMAT_R16G16B16A16_SNORM;
        case PixelFormat::R16G16B16A16_SINT:
            return DXGI_FORMAT_R16G16B16A16_SINT;
        case PixelFormat::R32G32_FLOAT:
            return DXGI_FORMAT_R32G32_FLOAT;
        case PixelFormat::R32G32_UINT:
            return DXGI_FORMAT_R32G32_UINT;
        case PixelFormat::R32G32_SINT:
            return DXGI_FORMAT_R32G32_SINT;
        case PixelFormat::R10G10B10A2_UNORM:
            return DXGI_FORMAT_R10G10B10A2_UNORM;
        case PixelFormat::R10G10B10A2_UINT:
            return DXGI_FORMAT_R10G10B10A2_UINT;
        case PixelFormat::R11G11B10_FLOAT:
            return DXGI_FORMAT_R11G11B10_FLOAT;
        case PixelFormat::R8G8B8A8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case PixelFormat::R8G8B8A8_UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case PixelFormat::R8G8B8A8_UINT:
            return DXGI_FORMAT_R8G8B8A8_UINT;
        case PixelFormat::R8G8B8A8_SNORM:
            return DXGI_FORMAT_R8G8B8A8_SNORM;
        case PixelFormat::R8G8B8A8_SINT:
            return DXGI_FORMAT_R8G8B8A8_SINT;
        case PixelFormat::R16G16_FLOAT:
            return DXGI_FORMAT_R16G16_FLOAT;
        case PixelFormat::R16G16_UNORM:
            return DXGI_FORMAT_R16G16_UNORM;
        case PixelFormat::R16G16_UINT:
            return DXGI_FORMAT_R16G16_UINT;
        case PixelFormat::R16G16_SNORM:
            return DXGI_FORMAT_R16G16_SNORM;
        case PixelFormat::R16G16_SINT:
            return DXGI_FORMAT_R16G16_SINT;
        case PixelFormat::D32_FLOAT:
            return DXGI_FORMAT_D32_FLOAT;
        case PixelFormat::R32_FLOAT:
            return DXGI_FORMAT_R32_FLOAT;
        case PixelFormat::R32_UINT:
            return DXGI_FORMAT_R32_UINT;
        case PixelFormat::R32_SINT:
            return DXGI_FORMAT_R32_SINT;
        case PixelFormat::D24_UNORM_S8_UINT:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case PixelFormat::R24_UNORM_X8_TYPELESS:
            return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        case PixelFormat::R8G8_UNORM:
            return DXGI_FORMAT_R8G8_UNORM;
        case PixelFormat::R8G8_UINT:
            return DXGI_FORMAT_R8G8_UINT;
        case PixelFormat::R8G8_SNORM:
            return DXGI_FORMAT_R8G8_SNORM;
        case PixelFormat::R8G8_SINT:
            return DXGI_FORMAT_R8G8_SINT;
        case PixelFormat::R16_FLOAT:
            return DXGI_FORMAT_R16_FLOAT;
        case PixelFormat::D16_UNORM:
            return DXGI_FORMAT_D16_UNORM;
        case PixelFormat::R16_UNORM:
            return DXGI_FORMAT_R16_UNORM;
        case PixelFormat::R16_UINT:
            return DXGI_FORMAT_R16_UINT;
        case PixelFormat::R16_SNORM:
            return DXGI_FORMAT_R16_SNORM;
        case PixelFormat::R16_SINT:
            return DXGI_FORMAT_R16_SINT;
        case PixelFormat::R8_UNORM:
            return DXGI_FORMAT_R8_UNORM;
        case PixelFormat::R8_UINT:
            return DXGI_FORMAT_R8_UINT;
        case PixelFormat::R8_SNORM:
            return DXGI_FORMAT_R8_SNORM;
        case PixelFormat::R8_SINT:
            return DXGI_FORMAT_R8_SINT;
        case PixelFormat::A8_UNORM:
            return DXGI_FORMAT_A8_UNORM;
        case PixelFormat::R1_UNORM:
            return DXGI_FORMAT_R1_UNORM;
        case PixelFormat::BC1_UNORM:
            return DXGI_FORMAT_BC1_UNORM;
        case PixelFormat::BC1_UNORM_SRGB:
            return DXGI_FORMAT_BC1_UNORM_SRGB;
        case PixelFormat::BC2_UNORM:
            return DXGI_FORMAT_BC2_UNORM;
        case PixelFormat::BC2_UNORM_SRGB:
            return DXGI_FORMAT_BC2_UNORM_SRGB;
        case PixelFormat::BC3_UNORM:
            return DXGI_FORMAT_BC3_UNORM;
        case PixelFormat::BC3_UNORM_SRGB:
            return DXGI_FORMAT_BC3_UNORM_SRGB;
        case PixelFormat::BC4_UNORM:
            return DXGI_FORMAT_BC4_UNORM;
        case PixelFormat::BC4_SNORM:
            return DXGI_FORMAT_BC4_SNORM;
        case PixelFormat::BC5_UNORM:
            return DXGI_FORMAT_BC5_UNORM;
        case PixelFormat::BC5_SNORM:
            return DXGI_FORMAT_BC5_SNORM;
        case PixelFormat::B5G6R5_UNORM:
            return DXGI_FORMAT_B5G6R5_UNORM;
        case PixelFormat::B5G5R5A1_UNORM:
            return DXGI_FORMAT_B5G5R5A1_UNORM;
        case PixelFormat::B8G8R8A8_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case PixelFormat::B8G8R8A8_UNORM_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        default:
            BIOME_FAIL();
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    static const char* ToNativeInputSemanticName(InputLayoutSemantic semantic)
    {
        switch (semantic)
        {
        case InputLayoutSemantic::Position:
            return "Position";
        case InputLayoutSemantic::Normal:
            return "Normal";
        case InputLayoutSemantic::Tangent:
            return "Tangent";
        case InputLayoutSemantic::UV:
        case InputLayoutSemantic::Float2:
        case InputLayoutSemantic::Float3:
        case InputLayoutSemantic::Float4:
            return "TEXCOORD";
        default:
            return nullptr;
        }
    }
}

void device::StartFrame(
    [[maybe_unused]] GpuDeviceHandle deviceHdl, 
    [[maybe_unused]] CommandQueueHandle cmdQueueHdl, 
    [[maybe_unused]] CommandBufferHandle cmdBufferHdl)
{

}

void device::EndFrame(
    GpuDeviceHandle deviceHdl, 
    CommandQueueHandle cmdQueueHdl, 
    CommandBufferHandle cmdBufferHdl)
{
    GpuDevice* pGpuDevice;
    AsType(pGpuDevice, deviceHdl);

    ID3D12CommandQueue* pCmdQueue;
    AsType(pCmdQueue, cmdQueueHdl);

    const uint64_t currentFrame = pGpuDevice->m_currentFrame;
    BIOME_ASSERT_ALWAYS_EXEC(SUCCEEDED(pCmdQueue->Signal(pGpuDevice->m_pFrameFence.Get(), currentFrame)));

    const uint32_t framesOfLatency = pGpuDevice->m_framesOfLatency;
    if (currentFrame >= framesOfLatency)
    {
        ID3D12Fence* pFrameFence = pGpuDevice->m_pFrameFence.Get();
        const uint64_t waitFrame = currentFrame - framesOfLatency;
        const HANDLE fenceEvent = pGpuDevice->m_fenceEvent;

        if (pFrameFence->GetCompletedValue() < waitFrame)
        {
            pFrameFence->SetEventOnCompletion(waitFrame, fenceEvent);
            WaitForSingleObject(fenceEvent, INFINITE);
        }
    }

    const uint64_t nextFrame = ++pGpuDevice->m_currentFrame;
    const uint64_t nextAllocatorIndex = nextFrame % (pGpuDevice->m_framesOfLatency + 1);

    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);

    ID3D12CommandAllocator* pCmdAllocator = pCmdBuffer->m_cmdAllocators[nextAllocatorIndex].Get();
    ID3D12GraphicsCommandList* pCmdList = pCmdBuffer->m_pCmdList.Get();

    pCmdAllocator->Reset();
    pCmdList->Reset(pCmdAllocator, nullptr);
}

GpuDeviceHandle device::CreateDevice(uint32_t framesOfLatency)
{
    GpuDeviceHandle deviceHdl = Handle_NULL;
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    if (SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory))))
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        if (GetHardwareAdapter(factory.Get(), &hardwareAdapter))
        {
            ComPtr<ID3D12Device> pDevice {};
            ComPtr<ID3D12DescriptorHeap> pRtvDescriptorHeap {};
            ComPtr<ID3D12Fence> pFrameFence {};

            if (SUCCEEDED(D3D12CreateDevice(
                hardwareAdapter.Get(),
                D3D_FEATURE_LEVEL_12_1,
                IID_PPV_ARGS(pDevice.ReleaseAndGetAddressOf()))))
            {
                D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
                rtvHeapDesc.NumDescriptors = framesOfLatency + 1;
                rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

                if (SUCCEEDED(pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(pRtvDescriptorHeap.ReleaseAndGetAddressOf()))))
                {
                    if (SUCCEEDED(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(pFrameFence.ReleaseAndGetAddressOf()))))
                    {
                        const HANDLE fenceEvent = CreateEvent(
                            nullptr,    // Security attributes
                            FALSE,      // Manual reset
                            FALSE,      // Initial state
                            L"FrameFenceEvent");

                        GpuDevice* pGpuDevice = new GpuDevice();
                        pGpuDevice->m_pDevice = pDevice;
                        pGpuDevice->m_pRtvDescriptorHeap = pRtvDescriptorHeap;
                        pGpuDevice->m_pFrameFence = pFrameFence;
                        pGpuDevice->m_fenceEvent = fenceEvent;
                        pGpuDevice->m_framesOfLatency = framesOfLatency;
                        pGpuDevice->m_rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

                        AsHandle(pGpuDevice, deviceHdl);
                    }
                }
            }
        }
    }

    return deviceHdl;
}

CommandQueueHandle device::CreateCommandQueue(GpuDeviceHandle deviceHdl, CommandType type)
{
    CommandQueueHandle cmdQueueHandle = Handle_NULL;
    GpuDevice* pGpuDevice;
    AsType(pGpuDevice, deviceHdl);
    ID3D12Device* pDevice = pGpuDevice->m_pDevice.Get();

    D3D12_COMMAND_LIST_TYPE cmdType = ToNativeCmdType(type);

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = cmdType;

    ID3D12CommandQueue* pCmdQueue;
    if (SUCCEEDED(pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCmdQueue))))
    {
        AsHandle(pCmdQueue, cmdQueueHandle);
    }

    return cmdQueueHandle;
}

CommandBufferHandle device::CreateCommandBuffer(GpuDeviceHandle deviceHdl, CommandType type)
{
    GpuDevice* pGpuDevice;
    AsType(pGpuDevice, deviceHdl);
    ID3D12Device* pDevice = pGpuDevice->m_pDevice.Get();
    D3D12_COMMAND_LIST_TYPE cmdType = ToNativeCmdType(type);

    CommandBuffer* pCmdBuffer = new CommandBuffer();

    const uint32_t allocatorCount = pGpuDevice->m_framesOfLatency + 1;
    CommandBuffer::AllocatorArray cmdAllocators(allocatorCount);

    for (uint32_t i = 0; i < allocatorCount; ++i)
    {
        cmdAllocators[i] = CreateCommandAllocator(pDevice, cmdType);
    }

    pCmdBuffer->m_pCmdList = CreateCommandList(pDevice, cmdAllocators[0].Get(), cmdType);
    pCmdBuffer->m_cmdAllocators = std::move(cmdAllocators);

    CommandBufferHandle cmdBufferHdl;
    AsHandle(pCmdBuffer, cmdBufferHdl);

    return cmdBufferHdl;
}

SwapChainHandle device::CreateSwapChain(
    GpuDeviceHandle deviceHdl, 
    CommandQueueHandle cmdQueueHdl, 
    WindowHandle windowHdl,
    uint32_t pixelWidth, 
    uint32_t pixelHeight)
{
    GpuDevice* pGpuDevice;
    AsType(pGpuDevice, deviceHdl);

    const uint32_t backBufferCount = pGpuDevice->m_framesOfLatency + 1;
    ID3D12CommandQueue* pCmdQueue;
    HWND windowHWND;

    AsType(pCmdQueue, cmdQueueHdl);
    AsType(windowHWND, windowHdl);

    SwapChainHandle swapChainHdl = Handle_NULL;

    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        // Enable additional debug layers.
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    if (SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory))))
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        if (GetHardwareAdapter(factory.Get(), &hardwareAdapter))
        {
            // Describe and create the swap chain.
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
            swapChainDesc.BufferCount = backBufferCount;
            swapChainDesc.Width = pixelWidth;
            swapChainDesc.Height = pixelHeight;
            swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapChainDesc.SampleDesc.Count = 1;

            IDXGISwapChain1* pSwapChain;
            if(SUCCEEDED(factory->CreateSwapChainForHwnd(
                pCmdQueue,        // Swap chain needs the queue so that it can force a flush on it.
                windowHWND,
                &swapChainDesc,
                nullptr,
                nullptr,
                &pSwapChain
            )))
            {
                // This sample does not support fullscreen transitions.
                BIOME_ASSERT_ALWAYS_EXEC(SUCCEEDED(factory->MakeWindowAssociation(windowHWND, DXGI_MWA_NO_ALT_ENTER)));
                SwapChain* pInternalSwapChain = new SwapChain();
                pInternalSwapChain->m_pSwapChain = pSwapChain;
                pInternalSwapChain->m_pixelWidth = pixelWidth;
                pInternalSwapChain->m_pixelHeight = pixelHeight;

                D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(pGpuDevice->m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

                StaticArray<TextureHandle> backBufferHandles(backBufferCount);
                for (uint32_t i = 0; i < backBufferCount; ++i)
                {
                    ID3D12Resource* pResource;
                    const HRESULT hr = pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pResource));
                    if (SUCCEEDED(hr))
                    {
                        pGpuDevice->m_pDevice->CreateRenderTargetView(pResource, nullptr, rtvHandle);

                        Texture* pTexture = new Texture();
                        pTexture->m_pResource = pResource;
                        pTexture->m_rtvHandle = rtvHandle;
                        AsHandle(pTexture, backBufferHandles[i]);

                        rtvHandle.ptr += pGpuDevice->m_rtvDescriptorSize;
                    }
                }

                pInternalSwapChain->m_backBuffers = std::move(backBufferHandles);
                AsHandle(pInternalSwapChain, swapChainHdl);
            }
        }
    }

    return swapChainHdl;
}

uint64_t device::GetCurrentFrameIndex(GpuDeviceHandle deviceHdl)
{
    GpuDevice* pGpuDevice;
    AsType(pGpuDevice, deviceHdl);
    return pGpuDevice->m_currentFrame;
}

PixelFormat device::GetSwapChainFormat(SwapChainHandle /*hdl*/)
{
    return PixelFormat::R8G8B8A8_UNORM;
}

TextureHandle device::GetBackBuffer(GpuDeviceHandle deviceHdl, SwapChainHandle hdl)
{
    GpuDevice* pGpuDevice;
    AsType(pGpuDevice, deviceHdl);

    const uint64_t index = pGpuDevice->m_currentFrame % (pGpuDevice->m_framesOfLatency + 1);
    SwapChain* pInternalSwapChain;
    AsType(pInternalSwapChain, hdl);

    if (index < pInternalSwapChain->m_backBuffers.Size())
    {
        return pInternalSwapChain->m_backBuffers[index];
    }

    return Handle_NULL;
}

ShaderHandle device::CreateShader(GpuDeviceHandle /*deviceHdl*/, const char* pFilePath)
{
    return biome::filesystem::ReadFileContent(pFilePath);
}

ShaderResourceLayoutHandle device::CreateShaderResourceLayout(GpuDeviceHandle deviceHdl, const char* pFilePath)
{
    GpuDevice* pGpuDevice;
    AsType(pGpuDevice, deviceHdl);

    size_t fileSize;
    ThreadHeapSmartPointer<uint8_t> layoutFileContent = filesystem::ReadFileContent(pFilePath, fileSize);

    if (layoutFileContent != nullptr)
    {
        ShaderResourceLayoutHandle layoutHdl;
        ID3D12RootSignature* pRootSig;
        ID3D12Device* pDevice = pGpuDevice->m_pDevice.Get();

        if (SUCCEEDED(pDevice->CreateRootSignature(0, layoutFileContent, fileSize, IID_PPV_ARGS(&pRootSig))))
        {
            AsHandle(pRootSig, layoutHdl);
            return layoutHdl;
        }
    }

    return Handle_NULL;
}

ShaderResourceLayoutHandle device::CreateShaderResourceLayout(GpuDeviceHandle deviceHdl, const ShaderResourceLayoutDesc& desc)
{
    GpuDevice* pGpuDevice;
    AsType(pGpuDevice, deviceHdl);

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC d3dRootSigDesc;
    d3dRootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;

    D3D12_ROOT_SIGNATURE_DESC1& d3dDesc = d3dRootSigDesc.Desc_1_1;
    d3dDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
    
    const size_t cStaticSamplerMaxCount = std::extent<decltype(desc.StaticSamplers)>::value;
    BIOME_ASSERT(desc.StaticSamplerCount <= cStaticSamplerMaxCount);

    D3D12_STATIC_SAMPLER_DESC staticSamplerDescs[cStaticSamplerMaxCount];
    for(uint32_t samplerIndex = 0; samplerIndex < desc.StaticSamplerCount; ++samplerIndex)
    {
        const SamplerStateDesc& samplerDesc = desc.StaticSamplers[samplerIndex];
        D3D12_STATIC_SAMPLER_DESC& d3dSamplerDesc = staticSamplerDescs[samplerIndex];
        d3dSamplerDesc.AddressU = ToNativeAddressMode(samplerDesc.m_AddressU);
        d3dSamplerDesc.AddressV = ToNativeAddressMode(samplerDesc.m_AddressV);
        d3dSamplerDesc.AddressW = ToNativeAddressMode(samplerDesc.m_AddressW);
        d3dSamplerDesc.Filter = ToNativeFilter(samplerDesc.m_Filter);
        d3dSamplerDesc.MaxAnisotropy = samplerDesc.m_MaxAnisotropy;
        d3dSamplerDesc.MinLOD = 0;
        d3dSamplerDesc.MaxLOD = 999;
        d3dSamplerDesc.RegisterSpace = 0;
        d3dSamplerDesc.ShaderRegister = samplerIndex;
        d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    }

    d3dDesc.pStaticSamplers = staticSamplerDescs;
    d3dDesc.NumStaticSamplers = desc.StaticSamplerCount;

    const uint32_t cParamCount = desc.ConstantCount + desc.ConstantBufferViewCount;
    d3dDesc.NumParameters = cParamCount;

    if(cParamCount > 0)
    {
        biome::data::Vector<D3D12_ROOT_PARAMETER1, memory::StackAllocator> rootParams(cParamCount, cParamCount);

        for(uint32_t constantIndex = 0; constantIndex < desc.ConstantCount; ++constantIndex)
        {
            D3D12_ROOT_PARAMETER1& param = rootParams[constantIndex];
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
            param.Constants.Num32BitValues = 1;
            param.Constants.RegisterSpace = 0;
            param.Constants.ShaderRegister = constantIndex;
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }

        for(uint32_t descriptorIndex = 0; descriptorIndex < desc.ConstantBufferViewCount; ++descriptorIndex)
        {
            const uint32_t cParamIndex = descriptorIndex + desc.ConstantCount;

            D3D12_ROOT_PARAMETER1& param = rootParams[cParamIndex];
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
            param.Descriptor.RegisterSpace = 0;
            param.Descriptor.ShaderRegister = cParamIndex;
            param.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
        }

        d3dDesc.pParameters = rootParams.Data();
    }

    ComPtr<ID3DBlob> pBlob, pErrBlob;
    if(SUCCEEDED(D3D12SerializeVersionedRootSignature(&d3dRootSigDesc, &pBlob, &pErrBlob)))
    {
        ShaderResourceLayoutHandle layoutHdl;
        ID3D12RootSignature* pRootSig;
        ID3D12Device* pDevice = pGpuDevice->m_pDevice.Get();

        if(SUCCEEDED(pDevice->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSig))))
        {
            AsHandle(pRootSig, layoutHdl);
            return layoutHdl;
        }
    }

    return Handle_NULL;
}

GfxPipelineHandle device::CreateGraphicsPipeline(GpuDeviceHandle deviceHdl, const GfxPipelineDesc& desc)
{
    ID3D12RootSignature* pRootSig;
    AsType(pRootSig, desc.ResourceLayout);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dDesc {};
    d3dDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    d3dDesc.pRootSignature = pRootSig;
    d3dDesc.VS.pShaderBytecode = desc.VertexShader.Data();
    d3dDesc.VS.BytecodeLength = desc.VertexShader.Size();
    d3dDesc.PS.pShaderBytecode = desc.FragmentShader.Data();
    d3dDesc.PS.BytecodeLength = desc.FragmentShader.Size();
    d3dDesc.NodeMask = 0;
    d3dDesc.SampleMask = 0;
    d3dDesc.DSVFormat = ToNativeFormat(desc.DepthFormat);
    d3dDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dDesc.SampleDesc.Count = 1;
    d3dDesc.SampleDesc.Quality = 0;

    d3dDesc.BlendState.AlphaToCoverageEnable = FALSE;
    d3dDesc.BlendState.IndependentBlendEnable = FALSE;

    const uint32_t cRTCount = desc.BlendState.RenderTargetCount;
    for(uint32_t rtIndex = 0; rtIndex < cRTCount; ++rtIndex)
    {
        D3D12_RENDER_TARGET_BLEND_DESC& d3dBlendDesc = d3dDesc.BlendState.RenderTarget[rtIndex];
        const BlendStateDesc& blendDesc = desc.BlendState;

        if(d3dBlendDesc.BlendEnable = desc.BlendState.IsEnabled)
        {
            d3dBlendDesc.BlendOp = ToNativeBlendOperation(blendDesc.ColorOperation);
            d3dBlendDesc.BlendOpAlpha = ToNativeBlendOperation(blendDesc.AlphaOperation);
            d3dBlendDesc.DestBlend = ToNativeBlend(blendDesc.DestinationColor);
            d3dBlendDesc.DestBlendAlpha = ToNativeBlend(blendDesc.DestinationAlpha);
            d3dBlendDesc.RenderTargetWriteMask = ToNativeWriteMask(blendDesc.ColorWrite, blendDesc.AlphaWrite);
            d3dBlendDesc.SrcBlend = ToNativeBlend(blendDesc.SourceColor);
            d3dBlendDesc.SrcBlendAlpha = ToNativeBlend(blendDesc.SourceAlpha);
        }
    }

    D3D12_RASTERIZER_DESC& d3dRasterDesc = d3dDesc.RasterizerState;
    d3dRasterDesc.AntialiasedLineEnable = FALSE;
    d3dRasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    d3dRasterDesc.CullMode = ToNativeCullMode(desc.RasterizerState.CullMode);
    d3dRasterDesc.DepthBias = 0;
    d3dRasterDesc.DepthBiasClamp = 0;
    d3dRasterDesc.DepthClipEnable = desc.RasterizerState.DepthClip;
    d3dRasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
    d3dRasterDesc.ForcedSampleCount = 0;
    d3dRasterDesc.FrontCounterClockwise = desc.RasterizerState.Winding == Winding::FrontCounterClockwise;
    d3dRasterDesc.MultisampleEnable = FALSE;
    d3dRasterDesc.SlopeScaledDepthBias = 0;

    d3dDesc.DepthStencilState.DepthEnable = desc.DepthStencilState.IsDepthTestEnabled;
    d3dDesc.DepthStencilState.StencilEnable = desc.DepthStencilState.IsStencilEnabled;
    d3dDesc.DepthStencilState.DepthFunc = ToNativeComparisonFunc(desc.DepthStencilState.DepthFunction);
    d3dDesc.DepthStencilState.BackFace.StencilFunc = ToNativeComparisonFunc(desc.DepthStencilState.BackFaceStencil.Function);
    d3dDesc.DepthStencilState.BackFace.StencilDepthFailOp = ToNativeStencilOp(desc.DepthStencilState.BackFaceStencil.DepthFailOp);
    d3dDesc.DepthStencilState.BackFace.StencilFailOp = ToNativeStencilOp(desc.DepthStencilState.BackFaceStencil.StencilFailOp);
    d3dDesc.DepthStencilState.BackFace.StencilPassOp = ToNativeStencilOp(desc.DepthStencilState.BackFaceStencil.DepthStencilPassOp);
    d3dDesc.DepthStencilState.FrontFace.StencilFunc = ToNativeComparisonFunc(desc.DepthStencilState.FrontFaceStencil.Function);
    d3dDesc.DepthStencilState.FrontFace.StencilDepthFailOp = ToNativeStencilOp(desc.DepthStencilState.FrontFaceStencil.DepthFailOp);
    d3dDesc.DepthStencilState.FrontFace.StencilFailOp = ToNativeStencilOp(desc.DepthStencilState.FrontFaceStencil.StencilFailOp);
    d3dDesc.DepthStencilState.FrontFace.StencilPassOp = ToNativeStencilOp(desc.DepthStencilState.FrontFaceStencil.DepthStencilPassOp);
    d3dDesc.DepthStencilState.DepthWriteMask = desc.DepthStencilState.IsDepthWriteEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    d3dDesc.DepthStencilState.StencilReadMask = desc.DepthStencilState.StencilReadMask;
    d3dDesc.DepthStencilState.StencilWriteMask = desc.DepthStencilState.StencilWriteMask;

    d3dDesc.NumRenderTargets = desc.RenderTargetCount;
    for(uint32_t rtIndex = 0; rtIndex < desc.RenderTargetCount; ++rtIndex)
    {
        d3dDesc.RTVFormats[rtIndex] = ToNativeFormat(desc.RenderTargetFormats[rtIndex]);
    }

    const uint32_t cInputElementCount = static_cast<uint32_t>(desc.InputLayout.Elements.Size());
    d3dDesc.InputLayout.NumElements = cInputElementCount;

    data::StaticArray<D3D12_INPUT_ELEMENT_DESC> d3dElements(cInputElementCount);
    for(uint32_t elementIndex = 0; elementIndex < cInputElementCount; ++elementIndex)
    {
        const InputLayoutElement& element = desc.InputLayout.Elements[elementIndex];
        D3D12_INPUT_ELEMENT_DESC& d3dElement = d3dElements[elementIndex];
        d3dElement.InputSlot = element.Slot;
        d3dElement.SemanticName = ToNativeInputSemanticName(element.Semantic);
    }

    d3dDesc.InputLayout.pInputElementDescs = d3dElements.Data();

    ID3D12PipelineState* pPipeline;
    GpuDevice* pGpuDevice;
    AsType(pGpuDevice, deviceHdl);
    ID3D12Device* pDevice = pGpuDevice->m_pDevice.Get();
    
    if (SUCCEEDED(pDevice->CreateGraphicsPipelineState(&d3dDesc, IID_PPV_ARGS(&pPipeline))))
    {
        GfxPipelineHandle gfxPipeHdl;
        AsHandle(pPipeline, gfxPipeHdl);

        return gfxPipeHdl;
    }

    return Handle_NULL;
}

ComputePipelineHandle device::CreateComputePipeline(GpuDeviceHandle /*deviceHdl*/, const ComputePipelineDesc& /*desc*/)
{
    return Handle_NULL;
}

DescriptorHeapHandle device::CreateDescriptorHeap(GpuDeviceHandle /*deviceHdl*/)
{
    return Handle_NULL;
}

void device::DestroyDevice(GpuDeviceHandle hdl)
{
    GpuDevice* pDevice;
    AsType(pDevice, hdl);
    CloseHandle(pDevice->m_fenceEvent);
    delete pDevice;
}

void device::DestroyCommandQueue(CommandQueueHandle hdl)
{
    ID3D12CommandQueue* pCmdQueue;
    AsType(pCmdQueue, hdl);
    pCmdQueue->Release();
}

void device::DestroyCommandBuffer(CommandBufferHandle cmdBufferHdl)
{
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);
    delete pCmdBuffer;
}

void device::DestroySwapChain(SwapChainHandle /*hdl*/)
{

}

void device::DestroyShaderResourceLayout(ShaderResourceLayoutHandle /*hdl*/)
{

}

void device::DestroyGfxPipeline(GfxPipelineHandle hdl)
{
    ID3D12PipelineState* pState;
    AsType(pState, hdl);
    pState->Release();
}

void device::DestroyComputePipeline(ComputePipelineHandle /*hdl*/)
{

}

void device::DestroyDescriptorHeap(DescriptorHeapHandle /*hdl*/)
{

}

void device::SignalFence(FenceHandle /*fenceHdl*/)
{

}

void device::GPUWaitOnFence(FenceHandle /*fenceHdl*/)
{

}

void device::CPUWaitOnFence(FenceHandle /*fenceHdl*/)
{

}

void device::ExecuteCommandBuffer(CommandQueueHandle cmdQueueHdl, CommandBufferHandle cmdBufferHdl)
{
    ID3D12CommandQueue* pCmdQueue;
    AsType(pCmdQueue, cmdQueueHdl);
    
    CommandBuffer* pCmdBuffer;
    AsType(pCmdBuffer, cmdBufferHdl);
    ID3D12CommandList* pCmdList = pCmdBuffer->m_pCmdList.Get();

    pCmdQueue->ExecuteCommandLists(1, &pCmdList);
}

void device::Present(SwapChainHandle swapChainHdl)
{
    SwapChain* pSwapChain;
    AsType(pSwapChain, swapChainHdl);

    DXGI_PRESENT_PARAMETERS params;
    params.DirtyRectsCount = 0;
    params.pDirtyRects = nullptr;
    params.pScrollOffset = nullptr;
    params.pScrollRect = nullptr;

    pSwapChain->m_pSwapChain->Present1(1, 0, &params);
}
