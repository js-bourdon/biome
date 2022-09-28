#include <pch.h>
#include "MemoryOffsetAllocator.h"
#include "VirtualMemoryAllocator.h"

using namespace biome::memory;

bool MemoryOffsetAllocator::Initialize(size_t byteSize, size_t pageSize)
{
    const size_t pageCount = Align(byteSize, pageSize) / pageSize;
    const size_t metadataOverhead = sizeof(Range) * pageCount * 2;

    m_pFreeRanges = static_cast<Range*>(VirtualMemoryAllocator::Allocate(metadataOverhead, metadataOverhead));
    m_pUsedRanges = m_pFreeRanges + pageCount;

    m_pFreeRanges->m_Offset = 0;
    m_pFreeRanges->m_Count = pageCount;
    m_FreeRangeCount = 1;

    m_SystemPageSize = pageSize;
    m_TotalPageCount = pageCount;

    return true;
}

bool MemoryOffsetAllocator::IsInitialized()
{
    return m_pFreeRanges != nullptr;
}

MemoryOffsetAllocator::~MemoryOffsetAllocator()
{
    Shutdown();
}

void MemoryOffsetAllocator::Shutdown()
{
    if (m_pFreeRanges != nullptr)
    {
        VirtualMemoryAllocator::Release(m_pFreeRanges);
    }
}

size_t MemoryOffsetAllocator::Allocate(size_t byteSize)
{
    BIOME_ASSERT_MSG(m_FreeRangeCount > 0, "Memory exhausted");

    const size_t requiredPageCount = Align(byteSize, m_SystemPageSize) / m_SystemPageSize;

    for (size_t i = m_FreeRangeCount - 1; i >= 0; --i)
    {
        Range& range = m_pFreeRanges[i];
        if (range.m_Count >= requiredPageCount)
        {
            Range usedRange { range.m_Offset - range.m_Count + requiredPageCount, requiredPageCount };
            AddUsedRange(usedRange);

            range.m_Count -= requiredPageCount;
            if (range.m_Count == 0)
            {
                Range& lastRange = m_pFreeRanges[m_FreeRangeCount - 1];
                range.m_Offset = lastRange.m_Offset;
                range.m_Count = lastRange.m_Count;
                lastRange.m_Offset = InvalidOffset;
                --m_FreeRangeCount;
            }

            return usedRange.m_Offset;
        }
    }

    BIOME_FAIL_MSG("Memory exhausted");
    return InvalidOffset;
}

bool MemoryOffsetAllocator::Release(size_t byteOffset)
{
    for (size_t i = 0; i < m_UsedRangeCount; ++i)
    {
        Range& range = m_pUsedRanges[i];
        if (range.m_Offset == byteOffset)
        {
            AddFreeRange(range);

            --m_UsedRangeCount;
            if (m_UsedRangeCount > 0)
            {
                Range& lastRange = m_pUsedRanges[m_UsedRangeCount];

                range.m_Offset = lastRange.m_Offset;
                range.m_Count = lastRange.m_Count;

                lastRange.m_Offset = InvalidOffset;
            }

            return true;
        }
    }

    return false;
}

void MemoryOffsetAllocator::AddFreeRange(const Range& range)
{
    BIOME_ASSERT(m_FreeRangeCount < m_TotalPageCount);
    AddRange(range, m_pFreeRanges, m_FreeRangeCount);
}

void MemoryOffsetAllocator::AddUsedRange(const Range& range)
{
    BIOME_ASSERT(m_UsedRangeCount < m_TotalPageCount);

    Range& newRange = m_pUsedRanges[m_UsedRangeCount];
    newRange.m_Offset = range.m_Offset;
    newRange.m_Count = range.m_Count;
    ++m_UsedRangeCount;


    AddRange(range, m_pUsedRanges, m_UsedRangeCount);
}

void MemoryOffsetAllocator::AddRange(const Range& range, Range* pRanges, size_t& rangeCount)
{
    bool merged = false;

    for (size_t i = rangeCount - 1; i >= 0; --i)
    {
        if (Merge(pRanges[i], range, pRanges[i]))
        {
            merged = true;
            break;
        }
    }

    if (!merged)
    {
        Range& newRange = pRanges[rangeCount];
        newRange.m_Offset = range.m_Offset;
        newRange.m_Count = range.m_Count;
        ++rangeCount;
    }

    for (size_t i = rangeCount - 1; i > 0; --i)
    {
        for (size_t j = i - 1; j >= 0; --j)
        {
            if (Merge(pRanges[i], pRanges[j], pRanges[j]))
            {
                --rangeCount;
                break;
            }
        }
    }
}

bool MemoryOffsetAllocator::AreOverlapping(const Range& range0, const Range& range1)
{
    const size_t rangeEnd0 = range0.m_Offset + range0.m_Count;
    const size_t rangeEnd1 = range1.m_Offset + range1.m_Count;

    return
        rangeEnd0 > range1.m_Offset ||
        rangeEnd1 > range0.m_Offset;
}

bool MemoryOffsetAllocator::CanMerge(const Range& range0, const Range& range1)
{
    BIOME_ASSERT(!AreOverlapping(range0, range1));

    const size_t rangeEnd0 = range0.m_Offset + range0.m_Count;
    const size_t rangeEnd1 = range1.m_Offset + range1.m_Count;

    return 
        rangeEnd0 == range1.m_Offset || 
        rangeEnd1 == range0.m_Offset;
}

bool MemoryOffsetAllocator::Merge(const Range& range0, const Range& range1, Range& newRange)
{
    if (CanMerge(range0, range1))
    {
        const size_t rangeEnd0 = range0.m_Offset + range0.m_Count;
        const size_t rangeEnd1 = range1.m_Offset + range1.m_Count;

        newRange.m_Count = range0.m_Count + range1.m_Count;

        if (rangeEnd0 == range1.m_Offset)
        {
            newRange.m_Offset = range0.m_Offset;
        }
        else
        {
            newRange.m_Offset = range1.m_Offset;
        }

        return true;
    }

    return false;
}
