#pragma once

namespace biome::render
{
    struct RenderPass;

    class Renderer final
    {
    public:

        Renderer() = default;

        void Initialize();
        void Render(const RenderPass* renderPass);
    };
}