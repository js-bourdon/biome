#include <pch.h>
#include "MemoryOffsetAllocator.h"
#include "VirtualMemoryAllocator.h"

using namespace biome::memory;

bool MemoryOffsetAllocator::Initialize(size_t byteSize, size_t pageSize)
{
    const uint32_t pageCount = static_cast<uint32_t>(Align(byteSize, pageSize)) / pageSize;
    const size_t metadataOverhead = (sizeof(uint32_t) + sizeof(FreeRange)) * pageCount;

    m_pFreeRanges = static_cast<FreeRange*>(VirtualMemoryAllocator::Allocate(metadataOverhead, metadataOverhead));
    m_pPages = reinterpret_cast<uint32_t*>(m_pFreeRanges + pageCount);

    m_SystemPageSize = pageSize;
    m_TotalPageCount = pageCount;
    m_NextPageIndex = 0;
}

bool MemoryOffsetAllocator::IsInitialized()
{
    return m_pFreeRanges != nullptr;
}
void MemoryOffsetAllocator::Shutdown()
{
    VirtualMemoryAllocator::Release(m_pFreeRanges);
}

void* MemoryOffsetAllocator::Allocate(size_t byteSize)
{
    const uint32_t requiredPageCount = static_cast<uint32_t>(Align(byteSize, m_SystemPageSize) / m_SystemPageSize);
    const uint32_t newNextPageIndex = m_NextPageIndex + requiredPageCount;
    BIOME_ASSERT_MSG(requiredPageCount <= m_TotalPageCount, "MemoryOffsetAllocator: Requested allocation exceeds allocator total byte size.");

    if (newNextPageIndex < m_CommittedPageCount)
    {
        m_pPages[m_NextPageIndex] = requiredPageCount;
        pAllocation = PageIndexToAddress(m_NextPageIndex);
        m_NextPageIndex = newNextPageIndex;
    }
    else
    {
        uint32_t freePagesStartIndex = SearchFreePages(requiredPageCount);
        if (freePagesStartIndex != UINT32_MAX)
        {
            m_pPages[freePagesStartIndex] = requiredPageCount;
            pAllocation = PageIndexToAddress(freePagesStartIndex);
        }
        else
        {
            uint32_t newRequiredCommitCount = newNextPageIndex - m_CommittedPageCount;
            uint32_t remainingReservedPagesCount = m_TotalPageCount - m_CommittedPageCount;
            if (newRequiredCommitCount <= remainingReservedPagesCount)
            {
                CommitMorePages(newRequiredCommitCount);

                m_pPages[m_NextPageIndex] = requiredPageCount;
                pAllocation = PageIndexToAddress(m_NextPageIndex);
                m_NextPageIndex = newNextPageIndex;
            }
        }
    }

    BIOME_ASSERT_MSG(pAllocation, "ThreadHeapAllocator: Out of Memory");

    return pAllocation;
}

bool MemoryOffsetAllocator::Release(void* pMemory)
{

}

size_t MemoryOffsetAllocator::AllocationSize(void* pMemory)
{

}

void MemoryOffsetAllocator::Defrag()
{
	// TODO
}

bool CanMerge(const FreeRange& range0, const FreeRange& range1)
{

}
