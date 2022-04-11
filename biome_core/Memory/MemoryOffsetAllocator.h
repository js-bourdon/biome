#pragma once

#include <stdint.h>
#include "biome_core/Memory/Memory.h"
#include "biome_core/Core/Defines.h"

namespace biome
{
    namespace memory
    {
        class MemoryOffsetAllocator
        {
        public:

            MemoryOffsetAllocator() = default;
            MemoryOffsetAllocator(const MemoryOffsetAllocator&) = delete;
            MemoryOffsetAllocator(MemoryOffsetAllocator&&) = delete;
            MemoryOffsetAllocator& operator=(const MemoryOffsetAllocator&) = delete;
            MemoryOffsetAllocator& operator=(MemoryOffsetAllocator&&) = delete;
            ~MemoryOffsetAllocator();

            [[nodiscard]]
            bool        Initialize(size_t byteSize, size_t pageSize);
            [[nodiscard]]
            bool        IsInitialized();
            void        Shutdown();
            void*       Allocate(size_t byteSize);
            bool        Release(void* pMemory);
            size_t      AllocationSize(void* pMemory);
            void        Defrag();

        private:

            struct FreeRange
            {
                uint32_t m_Index;
                uint32_t m_Count;
            };

            static bool CanMerge(const FreeRange& range0, const FreeRange& range1);

            FreeRange*  m_pFreeRanges { nullptr };
            uint32_t*   m_pPages { nullptr };
            size_t      m_SystemPageSize { 0 };
            uint32_t    m_TotalPageCount { 0 };
            uint32_t    m_NextPageIndex { 0 };
            uint32_t    m_FreeRangesCount { 0 };
        };
    }
}
