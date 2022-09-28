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

            static constexpr size_t InvalidOffset = std::numeric_limits<size_t>::max();

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
            size_t      Allocate(size_t byteSize);
            bool        Release(size_t byteOffset);

        private:

            struct Range
            {
                size_t m_Offset;
                size_t m_Count;
            };

            void AddFreeRange(const Range& range);
            void AddUsedRange(const Range& range);

            static bool AreOverlapping(const Range& range0, const Range& range1);
            static bool CanMerge(const Range& range0, const Range& range1);
            static bool Merge(const Range& range0, const Range& range1, Range& newRange);
            static void AddRange(const Range& range, Range* pRanges, size_t& rangeCount);
            static bool FindRange(size_t offset, Range* pRanges, size_t& rangeCount);

            Range*      m_pFreeRanges { nullptr };
            Range*      m_pUsedRanges { nullptr };
            size_t      m_SystemPageSize { 0 };
            size_t      m_TotalPageCount { 0 };
            size_t      m_FreeRangeCount { 0 };
            size_t      m_UsedRangeCount { 0 };
        };
    }
}
