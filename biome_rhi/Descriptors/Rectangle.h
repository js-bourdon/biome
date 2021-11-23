#pragma once

#include <stdint.h>

namespace biome::rhi
{
    struct Rectangle
    {
        uint32_t m_left;
        uint32_t m_top;
        uint32_t m_right;
        uint32_t m_bottom;
    };
}