#include <pch.h>
#include "biome_render/Renderer.h"
#include "biome_render/RenderPass.h"

using namespace biome::render;

void Renderer::Initialize()
{
    /*WindowHandle hwnd = factory::CreateNewWindow(reinterpret_cast<biome::rhi::AppHandle>(hInstance), windowWidth, windowHeight, L"Biome");
    factory::DisplayWindow(hwnd);

    constexpr uint32_t framesOfLatency = 1;
    const GpuDeviceHandle deviceHdl = device::CreateDevice(framesOfLatency);
    const CommandQueueHandle cmdQueueHdl = device::CreateCommandQueue(deviceHdl, CommandType::Graphics);*/
}

void Renderer::Render(const RenderPass* renderPass)
{

}
