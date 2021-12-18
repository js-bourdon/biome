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

    WindowHandle hwnd = factory::CreateNewWindow(reinterpret_cast<biome::rhi::AppHandle>(hInstance), 1920, 1080, L"Biome");
    factory::DisplayWindow(hwnd);

    GpuDeviceHandle deviceHdl = device::CreateDevice();
    CommandQueueHandle cmdQueueHdl = device::CreateCommandQueue(deviceHdl, CommandType::Graphics);
    
    // Assets loading
    AssetDatabase* pAssetDb = LoadDatabase("Media/builds/star_trek_danube_class/StartTrek.db");
    const Texture* pTextures = GetTextures(pAssetDb);
    const Mesh* pMeshes = GetMeshes(pAssetDb);
    DestroyDatabase(pAssetDb);

    while (!biome::rhi::events::PumpMessages())
    {
        std::this_thread::sleep_for(100ms);
    }

    device::DestroyDevice(deviceHdl);
    device::DestroyCommandQueue(cmdQueueHdl);

    return 0;
}
