#pragma once

#include <stdint.h>
#include "biome_core/Memory/Memory.h"

namespace biome
{
    namespace memory
    {
        class StackAllocator
        {
        public:

            StackAllocator() = default;
            ~StackAllocator() = default;

            static void*    Allocate(size_t byteSize) { return biome::memory::StackAlloc(byteSize); }
            static void     Release(void* /*pMemory*/) {}
        };
    }
}