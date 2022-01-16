// LWGL_TestApp.cpp : Defines the entry point for the application.
//
#include "framework.h"
#include "TestApp.h"
#include <cstdint>
#include <thread>
#include <chrono>
#include "biome_rhi/WindowFactory/WindowFactory.h"
#include "biome_rhi/Events/MessagePump.h"
#include "biome_rhi/Resources/ResourceHandles.h"
#include "biome_core/SystemInfo/SystemInfo.h"
#include "biome_core/Memory/VirtualMemoryAllocator.h"
#include "biome_core/Memory/ThreadHeapAllocator.h"
#include "biome_core/Threading/WorkerThread.h"
#include "biome_core/Assets/AssetDatabase.h"
#include "biome_rhi/Systems/DeviceSystem.h"
#include "biome_rhi/Systems/CommandSystem.h"
#include "biome_rhi/Descriptors/PipelineDesc.h"
#include "biome_rhi/Descriptors/Viewport.h"
#include "biome_rhi/Descriptors/Rectangle.h"

using namespace biome::rhi;
using namespace biome::memory;
using namespace biome::threading;
using namespace biome::asset;
using namespace std::chrono_literals;

static int WorkerFunction(int iterationCount)
{
    int total = 0;
    for (int i = 0; i < iterationCount; ++i)
    {
        total += i;
    }

    return total;
}

typedef int (WorkerFnctType)(int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    BIOME_ASSERT_ALWAYS_EXEC(ThreadHeapAllocator::Initialize(GiB(1), MiB(100)));

    WorkerThread<WorkerFnctType> worker0(WorkerFunction, GiB(1), MiB(100));
    WorkerThread<WorkerFnctType> worker1(WorkerFunction, GiB(1), MiB(100));
    worker0.Init();
    worker1.Init();

    worker0.Run(1000);
    worker1.Run(100);

    int value0 = worker0.Wait();
    int value1 = worker1.Wait();

    constexpr uint32_t windowWidth = 1980;
    constexpr uint32_t windowHeight = 1080;

    WindowHandle hwnd = factory::CreateNewWindow(reinterpret_cast<biome::rhi::AppHandle>(hInstance), windowWidth, windowHeight, L"Biome");
    factory::DisplayWindow(hwnd);

    constexpr uint32_t framesOfLatency = 1;
    const GpuDeviceHandle deviceHdl = device::CreateDevice(framesOfLatency);
    const CommandQueueHandle cmdQueueHdl = device::CreateCommandQueue(deviceHdl, CommandType::Graphics);
    
    // Assets loading
    AssetDatabase* pAssetDb = LoadDatabase("Media/builds/star_trek_danube_class/StartTrek.db");
    const Texture* pTextures = GetTextures(pAssetDb);
    const Mesh* pMeshes = GetMeshes(pAssetDb);
    DestroyDatabase(pAssetDb);

    constexpr uint32_t backBufferCount = 2;
    const SwapChainHandle swapChainHdl = device::CreateSwapChain(deviceHdl, cmdQueueHdl, hwnd, windowWidth, windowHeight);

    const CommandBufferHandle cmdBufferHdl = device::CreateCommandBuffer(deviceHdl, CommandType::Graphics);
    BIOME_ASSERT(cmdBufferHdl != biome::Handle_NULL);

    const ShaderResourceLayoutHandle rscLayoutHdl = device::CreateShaderResourceLayout(deviceHdl, "Shaders/bin/root_signature.cso");
    BIOME_ASSERT(rscLayoutHdl != biome::Handle_NULL);

    // TODO: Make actual handles!!!
    ShaderHandle vertexShader = device::CreateShader(deviceHdl, "Shaders/bin/fullscreen_vs.cso");
    ShaderHandle pixelShader = device::CreateShader(deviceHdl, "Shaders/bin/fullscreen_ps.cso");

    descriptors::GfxPipelineDesc pipelineDesc{};
    pipelineDesc.ResourceLayout = rscLayoutHdl;
    pipelineDesc.VertexShader = std::move(vertexShader);
    pipelineDesc.FragmentShader = std::move(pixelShader);
    pipelineDesc.RenderTargetFormats[0] = device::GetSwapChainFormat(swapChainHdl);
    pipelineDesc.RenderTargetCount = 1;
    pipelineDesc.BlendState.IsEnabled = false;
    pipelineDesc.RasterizerState.DepthClip = false;

    const GfxPipelineHandle gfxPipeHdl = device::CreateGraphicsPipeline(deviceHdl, pipelineDesc);
    BIOME_ASSERT(gfxPipeHdl != biome::Handle_NULL);

    biome::rhi::Rectangle scissorRect;
    scissorRect.m_left = 0;
    scissorRect.m_right = windowWidth;
    scissorRect.m_top = 0;
    scissorRect.m_bottom = windowHeight;

    biome::rhi::Viewport viewport;
    viewport.m_x = 0;
    viewport.m_y = 0;
    viewport.m_width = windowWidth;
    viewport.m_height = windowHeight;
    viewport.m_minDepth = 0.f;
    viewport.m_maxDepth = 1.f;

    while (!biome::rhi::events::PumpMessages())
    {
        device::StartFrame(deviceHdl, cmdQueueHdl, cmdBufferHdl);

        const TextureHandle backBufferHdl = device::GetBackBuffer(deviceHdl, swapChainHdl);

        commands::SetGraphicsShaderResourceLayout(cmdBufferHdl, rscLayoutHdl);
        commands::SetGfxPipeline(cmdBufferHdl, gfxPipeHdl);
        commands::SetPrimitiveTopology(cmdBufferHdl, PrimitiveTopology::TriangleList);
        commands::RSSetScissorRects(cmdBufferHdl, 1, &scissorRect);
        commands::RSSetViewports(cmdBufferHdl, 1, &viewport);

        commands::TextureStateTransition transition;
        transition.m_textureHdl = backBufferHdl;
        transition.m_before = ResourceState::Present;
        transition.m_after = ResourceState::RenderTarget;
        commands::ResourceTransition(cmdBufferHdl, &transition, 1);

        commands::ClearRenderTarget(cmdBufferHdl, backBufferHdl, { 0.f, 0.5f, 0.f ,0.f });
        commands::OMSetRenderTargets(cmdBufferHdl, 1, &backBufferHdl, nullptr);
        commands::DrawInstanced(cmdBufferHdl, 3, 1, 0, 0);

        transition.m_before = ResourceState::RenderTarget;
        transition.m_after = ResourceState::Present;
        commands::ResourceTransition(cmdBufferHdl, &transition, 1);

        commands::CloseCommandBuffer(cmdBufferHdl);
        device::ExecuteCommandBuffer(cmdQueueHdl, cmdBufferHdl);

        device::Present(swapChainHdl);

        device::EndFrame(deviceHdl, cmdQueueHdl, cmdBufferHdl);

        std::this_thread::sleep_for(100ms);
    }

    device::DestroyGfxPipeline(gfxPipeHdl);
    device::DestroyShaderResourceLayout(rscLayoutHdl);
    device::DestroyCommandBuffer(cmdBufferHdl);
    device::DestroySwapChain(swapChainHdl);
    device::DestroyCommandQueue(cmdQueueHdl);
    device::DestroyDevice(deviceHdl);

    return 0;
}
