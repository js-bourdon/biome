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
#include "biome_core/FileSystem/FileSystem.h"
#include "biome_core/Time/Timer.h"
#include "biome_rhi/Systems/DeviceSystem.h"
#include "biome_rhi/Systems/CommandSystem.h"
#include "biome_rhi/Descriptors/PipelineDesc.h"
#include "biome_rhi/Descriptors/Viewport.h"
#include "biome_rhi/Descriptors/Rectangle.h"
#include "biome_render/FirstPersonCamera.h"

#ifdef _DEBUG
    #include <pix3.h>
#endif

using namespace biome::rhi;
using namespace biome::memory;
using namespace biome::threading;
using namespace biome::asset;
using namespace biome::render;
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

// TODO: Remove hardcoded 256
struct alignas(256) Constants
{
    biome::math::Matrix4x4 ViewMatrix {};
    biome::math::Matrix4x4 ProjectionMatrix {};
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    //LoadLibrary(L"..\\packages\\WinPixEventRuntime.1.0.240308001\\bin\\x64\\WinPixEventRuntime.dll");

    BIOME_ASSERT_ALWAYS_EXEC(ThreadHeapAllocator::Initialize(GiB(1), MiB(100)));

    WorkerThread<WorkerFnctType> worker0(WorkerFunction, GiB(1), MiB(100));
    WorkerThread<WorkerFnctType> worker1(WorkerFunction, GiB(1), MiB(100));
    worker0.Init();
    worker1.Init();

    worker0.Run(1000);
    const int value0 = worker0.Wait();
    BIOME_ASSERT(value0 == 499500);

    worker1.Run(100);
    const int value1 = worker1.Wait();
    BIOME_ASSERT(value1 == 4950);

    constexpr uint32_t windowWidth = 1980;
    constexpr uint32_t windowHeight = 1080;

    const WindowHandle hwnd = factory::CreateNewWindow(reinterpret_cast<biome::rhi::AppHandle>(hInstance), windowWidth, windowHeight, L"Biome");
    factory::DisplayWindow(hwnd);

    constexpr uint32_t framesOfLatency = 1;
    constexpr bool useCpuEmulation = false;
    const GpuDeviceHandle deviceHdl = device::CreateDevice(framesOfLatency, useCpuEmulation);
    
    // Assets loading
    AssetDatabase* pAssetDb = LoadDatabase("Media/builds/star_trek_danube_class/StartTrek.db");
    const Texture* pTextures = GetTextures(pAssetDb);
    const Mesh* pMeshes = GetMeshes(pAssetDb);
    BIOME_ASSERT(pAssetDb->m_header.m_meshCount > 0);
    BIOME_ASSERT(pAssetDb->m_header.m_textureCount > 0);

    const Mesh& mesh = pMeshes[0];
    BIOME_ASSERT(mesh.m_subMeshCount > 0);

    const SubMesh& subMesh = mesh.m_subMeshes[0];
    const BufferView indexBuffer = subMesh.m_indexBuffer;
    BIOME_ASSERT(subMesh.m_streamCount > 0);
    const VertexStream& vertexBufferPos = subMesh.m_streams[0];
    const VertexStream& vertexBufferNormal = subMesh.m_streams[1];
    const VertexStream& vertexBufferUv = subMesh.m_streams[3];

    const StaticArray<uint8_t> buffers = biome::filesystem::ReadFileContent("Media/builds/star_trek_danube_class/Buffers.bin");
    const StaticArray<uint8_t> textures = biome::filesystem::ReadFileContent("Media/builds/star_trek_danube_class/Textures.bin");

    const BufferHandle indexBufferHdl = 
        device::CreateBuffer(
            deviceHdl, 
            BufferType::Index, 
            static_cast<uint32_t>(indexBuffer.m_byteSize), 
            static_cast<uint32_t>(indexBuffer.m_byteStride),
            Format::R32_UINT);

    const BufferHandle vertexBufferPosHdl = 
        device::CreateBuffer(
            deviceHdl, 
            BufferType::Vertex, 
            static_cast<uint32_t>(vertexBufferPos.m_byteSize), 
            static_cast<uint32_t>(vertexBufferPos.m_byteStride), 
            Format::R32G32B32_FLOAT);

    const BufferHandle vertexBufferNormalHdl = 
        device::CreateBuffer(
            deviceHdl, 
            BufferType::Vertex, 
            static_cast<uint32_t>(vertexBufferNormal.m_byteSize), 
            static_cast<uint32_t>(vertexBufferNormal.m_byteStride), 
            Format::R32G32B32_FLOAT);

    const BufferHandle vertexBufferUvHdl =
        device::CreateBuffer(
            deviceHdl,
            BufferType::Vertex,
            static_cast<uint32_t>(vertexBufferUv.m_byteSize),
            static_cast<uint32_t>(vertexBufferUv.m_byteStride),
            Format::R32G32_FLOAT);

    const BufferHandle constantBufferHdl = 
        device::CreateBuffer(
            deviceHdl, 
            BufferType::Constant, 
            static_cast<uint32_t>(sizeof(Constants)), 
            0u, 
            Format::Unknown);

    constexpr bool allowRtv = false;
    constexpr bool allowDsv = true;
    constexpr bool allowUav = false;
    constexpr Format dsvFormat = Format::D24_UNORM_S8_UINT;

    const TextureHandle depthBufferHdl = device::CreateTexture(deviceHdl, windowWidth, windowHeight, dsvFormat, allowRtv, allowDsv, allowUav);

    void* const pIndexBufferData = device::MapBuffer(deviceHdl, indexBufferHdl);
    void* const pVertexBufferPosData = device::MapBuffer(deviceHdl, vertexBufferPosHdl);
    void* const pVertexBufferNormalData = device::MapBuffer(deviceHdl, vertexBufferNormalHdl);
    void* const pVertexBufferUvData = device::MapBuffer(deviceHdl, vertexBufferUvHdl);

    memcpy(pIndexBufferData, buffers.Data() + indexBuffer.m_byteOffset, indexBuffer.m_byteSize);
    memcpy(pVertexBufferPosData, buffers.Data() + vertexBufferPos.m_byteOffset, vertexBufferPos.m_byteSize);
    memcpy(pVertexBufferNormalData, buffers.Data() + vertexBufferNormal.m_byteOffset, vertexBufferNormal.m_byteSize);
    memcpy(pVertexBufferUvData, buffers.Data() + vertexBufferUv.m_byteOffset, vertexBufferUv.m_byteSize);

    device::UnmapBuffer(deviceHdl, indexBufferHdl);
    device::UnmapBuffer(deviceHdl, vertexBufferPosHdl);
    device::UnmapBuffer(deviceHdl, vertexBufferNormalHdl);
    device::UnmapBuffer(deviceHdl, vertexBufferUvHdl);

    constexpr uint32_t backBufferCount = 2;
    const SwapChainHandle swapChainHdl = device::CreateSwapChain(deviceHdl, hwnd, windowWidth, windowHeight);

    const CommandBufferHandle cmdBufferHdl = device::CreateCommandBuffer(deviceHdl, CommandType::Graphics);
    BIOME_ASSERT(cmdBufferHdl != biome::Handle_NULL);

    const ShaderResourceLayoutHandle rscLayoutHdl = device::CreateShaderResourceLayout(deviceHdl, "Shaders/bin/root_signature.cso");
    BIOME_ASSERT(rscLayoutHdl != biome::Handle_NULL);

    // TODO: Make actual handles!!!
    //ShaderHandle vertexShader = device::CreateShader(deviceHdl, "Shaders/bin/fullscreen_vs.cso");
    ShaderHandle vertexShader = device::CreateShader(deviceHdl, "Shaders/bin/basic_vs.cso");
    //ShaderHandle pixelShader = device::CreateShader(deviceHdl, "Shaders/bin/fullscreen_ps.cso");
    ShaderHandle pixelShader = device::CreateShader(deviceHdl, "Shaders/bin/basic_ps.cso");

    descriptors::GfxPipelineDesc pipelineDesc{};
    pipelineDesc.ResourceLayout = rscLayoutHdl;
    pipelineDesc.VertexShader = std::move(vertexShader);
    pipelineDesc.FragmentShader = std::move(pixelShader);
    pipelineDesc.RenderTargetFormats[0] = device::GetSwapChainFormat(swapChainHdl);
    pipelineDesc.RenderTargetCount = 1;
    pipelineDesc.BlendState.IsEnabled = false;
    pipelineDesc.DepthFormat = descriptors::Format::D24_UNORM_S8_UINT;
    pipelineDesc.DepthStencilState.DepthFunction = descriptors::ComparisonFunction::LESS_EQUAL;
    pipelineDesc.DepthStencilState.IsDepthTestEnabled = true;
    pipelineDesc.DepthStencilState.IsDepthWriteEnabled = true;

    pipelineDesc.InputLayout.Elements = StaticArray<InputLayoutElement>
    {
        InputLayoutElement
        {
            InputLayoutSemantic::Position,
            0, // SemanticIndex;
            0, // Slot;
            biome::rhi::descriptors::Format::R32G32B32_FLOAT
        },
        InputLayoutElement
        {
            InputLayoutSemantic::Normal,
            0, // SemanticIndex;
            1, // Slot;
            biome::rhi::descriptors::Format::R32G32B32_FLOAT
        },
        InputLayoutElement
        {
            InputLayoutSemantic::UV,
            0, // SemanticIndex;
            2, // Slot;
            biome::rhi::descriptors::Format::R32G32_FLOAT
        }
    };

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

    constexpr biome::math::Vector4 worldPos = { 0.f, 0.0f, -500.f, 1.f };
    constexpr biome::math::Vector4 lookAtWorldPos = { 0.f, 0.0f, 0.f, 1.f };
    constexpr float fov = biome::math::PI_ON_FOUR;
    constexpr float nearPlane = 0.1f;
    constexpr float farPlane = 5000.0f;
    const float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

    FirstPersonCamera camera = {};
    camera.Init(worldPos, lookAtWorldPos, fov, aspectRatio, nearPlane, farPlane);

    biome::rhi::events::MessageConsumer msgConsumer = { camera.GetMessageCallback(), &camera };
    biome::rhi::events::RegisterMessageConsumer(msgConsumer);

    biome::time::Timer timer = {};

    while (!biome::rhi::events::PumpMessages())
    {
        camera.FrameMove(timer.GetElapsedSecondsSinceLastCall());

        device::StartFrame(deviceHdl);

        // Perform any copy operation before OnResourceCopyDone.

        // Update constant buffer
        {
            Constants constants =
            {
                camera.GetViewMatrix(),
                camera.GetProjMatrix()
            };

            void* pConstantBufferData = device::MapBuffer(deviceHdl, constantBufferHdl);
            memcpy(pConstantBufferData, &constants, sizeof(constants));
            device::UnmapBuffer(deviceHdl, constantBufferHdl);
        }

        device::OnResourceUpdatesDone(deviceHdl);

        const TextureHandle backBufferHdl = device::GetBackBuffer(deviceHdl, swapChainHdl);

        commands::SetDescriptorHeaps(cmdBufferHdl);
        commands::SetGraphicsShaderResourceLayout(cmdBufferHdl, rscLayoutHdl);
        commands::SetGfxPipeline(cmdBufferHdl, gfxPipeHdl);
        commands::SetPrimitiveTopology(cmdBufferHdl, PrimitiveTopology::TriangleList);
        commands::RSSetScissorRects(cmdBufferHdl, 1, &scissorRect);
        commands::RSSetViewports(cmdBufferHdl, 1, &viewport);
        commands::SetGraphicsConstantBuffer(cmdBufferHdl, constantBufferHdl, 1);

        commands::TextureStateTransition transition;
        transition.m_textureHdl = backBufferHdl;
        transition.m_before = ResourceState::Present;
        transition.m_after = ResourceState::RenderTarget;
        commands::ResourceTransition(cmdBufferHdl, &transition, 1);

        commands::SetIndexBuffer(cmdBufferHdl, indexBufferHdl);

        const BufferHandle vertexStreams[] = { vertexBufferPosHdl, vertexBufferNormalHdl, vertexBufferUvHdl };
        commands::SetVertexBuffers(cmdBufferHdl, 0, std::extent<decltype(vertexStreams)>::value, vertexStreams);

        commands::ClearRenderTarget(cmdBufferHdl, backBufferHdl, { 0.f, 0.5f, 0.f ,0.f });
        commands::ClearDepthStencil(cmdBufferHdl, depthBufferHdl);
        commands::OMSetRenderTargets(cmdBufferHdl, 1, &backBufferHdl, &depthBufferHdl);
        commands::DrawIndexedInstanced(cmdBufferHdl, static_cast<uint32_t>(indexBuffer.m_byteSize >> 2u), 1u, 0u, 0u, 0u);

        transition.m_before = ResourceState::RenderTarget;
        transition.m_after = ResourceState::Present;
        commands::ResourceTransition(cmdBufferHdl, &transition, 1);

        commands::CloseCommandBuffer(cmdBufferHdl);
        device::ExecuteCommandBuffer(deviceHdl, cmdBufferHdl);

        device::Present(swapChainHdl);

        device::EndFrame(deviceHdl);

        //std::this_thread::sleep_for(100ms);
    }

    DestroyDatabase(pAssetDb);

    device::DrainPipeline(deviceHdl);
    device::DestroyGfxPipeline(gfxPipeHdl);
    device::DestroyShaderResourceLayout(rscLayoutHdl);
    device::DestroySwapChain(swapChainHdl);
    device::DestroyDevice(deviceHdl);

    return 0;
}
