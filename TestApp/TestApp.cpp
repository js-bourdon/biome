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
#include "biome_rhi/Descriptors/PipelineDesc.h"

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

    const GpuDeviceHandle deviceHdl = device::CreateDevice();
    const CommandQueueHandle cmdQueueHdl = device::CreateCommandQueue(deviceHdl, CommandType::Graphics);
    
    // Assets loading
    AssetDatabase* pAssetDb = LoadDatabase("Media/builds/star_trek_danube_class/StartTrek.db");
    const Texture* pTextures = GetTextures(pAssetDb);
    const Mesh* pMeshes = GetMeshes(pAssetDb);
    DestroyDatabase(pAssetDb);

    constexpr uint32_t backBufferCount = 2;
    const SwapChainHandle swapChainHdl = device::CreateSwapChain(deviceHdl, cmdQueueHdl, hwnd, backBufferCount, windowWidth, windowHeight);

    device::CommandBuffer cmdBufferHdl;
    BIOME_ASSERT_ALWAYS_EXEC(device::CreateCommandBuffer(deviceHdl, CommandType::Graphics, cmdBufferHdl));

    ShaderResourceLayoutHandle rscLayoutHdl = device::CreateShaderResourceLayout(deviceHdl, "Shaders/bin/root_signature.cso");
    BIOME_ASSERT(rscLayoutHdl != biome::Handle_NULL);

    Shader vertexShader = device::CreateShader(deviceHdl, "Shaders/bin/fullscreen_vs.cso");
    Shader pixelShader = device::CreateShader(deviceHdl, "Shaders/bin/fullscreen_ps.cso");

    descriptors::GfxPipelineDesc pipelineDesc{};
    pipelineDesc.ResourceLayout = rscLayoutHdl;
    pipelineDesc.VertexShader = std::move(vertexShader);
    pipelineDesc.FragmentShader = std::move(pixelShader);
    pipelineDesc.RenderTargetFormats[0] = device::GetSwapChainFormat(swapChainHdl);
    pipelineDesc.RenderTargetCount = 1;

    GfxPipelineHandle gfxPipeHdl = device::CreateGraphicsPipeline(deviceHdl, pipelineDesc);
    BIOME_ASSERT(gfxPipeHdl != biome::Handle_NULL);

    while (!biome::rhi::events::PumpMessages())
    {
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
