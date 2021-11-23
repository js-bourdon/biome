#pragma once

#include <cstdint>

namespace biome::rhi
{
    namespace descriptors
    {
        struct ScissorRectDesc
        {
            uint32_t m_Left;
            uint32_t m_Top;
            uint32_t m_Right;
            uint32_t m_Bottom;
        };
    }
}
