#pragma once

namespace biome::rhi
{
    struct Viewport
    {
        float m_x;
        float m_y;
        float m_width;
        float m_height;
        float m_minDepth;
        float m_maxDepth;
    };
}
