#include <pch.h>
#include "ThreadHeapAllocator.h"
#include "VirtualMemoryAllocator.h"

using namespace biome::memory;

thread_local ThreadHeapAllocator* ThreadHeapAllocator::s_pAllocator = nullptr;

ThreadHeapAllocator* ThreadHeapAllocator::CreateAllocator(size_t heapByteSize, size_t initialCommitByteSize)
{
    void* pMemory = malloc(sizeof(ThreadHeapAllocator));
    ThreadHeapAllocator* pAllocator = new(pMemory) ThreadHeapAllocator();

    const uint32_t pageSize = VirtualMemoryAllocator::GetSystemPageSize();
    const uint32_t pageCount = static_cast<uint32_t>(Align(heapByteSize, pageSize)) / pageSize;
    const uint32_t committedPageCount = static_cast<uint32_t>(Align(initialCommitByteSize, pageSize)) / pageSize;
    const size_t metadataOverhead = (sizeof(uint32_t) + sizeof(FreeRange)) * pageCount;

    pAllocator->m_MemoryPool = reinterpret_cast<uintptr_t>(VirtualMemoryAllocator::Allocate(heapByteSize, initialCommitByteSize, pageSize));
    pAllocator->m_pFreeRanges = static_cast<FreeRange*>(VirtualMemoryAllocator::Allocate(metadataOverhead, metadataOverhead));
    pAllocator->m_pPages = reinterpret_cast<uint32_t*>(pAllocator->m_pFreeRanges + pageCount);

    pAllocator->m_SystemPageSize = pageSize;
    pAllocator->m_TotalPageCount = pageCount;
    pAllocator->m_CommittedPageCount = committedPageCount;
    pAllocator->m_NextPageIndex = 0;

    return pAllocator;
}

ThreadHeapAllocator::~ThreadHeapAllocator()
{
    if (m_MemoryPool != 0)
    {
        VirtualMemoryAllocator::Release(m_pFreeRanges);
        VirtualMemoryAllocator::Release(reinterpret_cast<void*>(m_MemoryPool));
    }
}

bool ThreadHeapAllocator::Initialize(size_t heapByteSize, size_t initialCommitByteSize)
{
    BIOME_ASSERT_MSG(s_pAllocator == nullptr, "ThreadHeapAllocator already initialized");

    s_pAllocator = CreateAllocator(heapByteSize, initialCommitByteSize);
    return s_pAllocator != nullptr;
}

bool ThreadHeapAllocator::IsInitialized()
{
    return s_pAllocator != nullptr;
}

void ThreadHeapAllocator::Shutdown()
{
    s_pAllocator->~ThreadHeapAllocator();
    free(s_pAllocator);
    s_pAllocator = nullptr;
}

void* ThreadHeapAllocator::Allocate(size_t byteSize)
{
    if(s_pAllocator)
    {
        return s_pAllocator->AllocateInternal(byteSize);
    }
    
    return nullptr;
}

bool ThreadHeapAllocator::WasAllocatedFromThisThread(void* pMemory)
{
    return s_pAllocator->WasAllocatedFromThisThreadInternal(pMemory);
}

bool ThreadHeapAllocator::Release(void *pMemory)
{
    if(s_pAllocator)
    {
        return s_pAllocator->ReleaseInternal(pMemory);
    }
    
    return false;
}

size_t ThreadHeapAllocator::AllocationSize(void *pMemory)
{
    return s_pAllocator->AllocationSizeInternal(pMemory);
}

void ThreadHeapAllocator::Defrag()
{
    s_pAllocator->DefragInternal();
}

void* ThreadHeapAllocator::AllocateInternal(size_t byteSize)
{
    void *pAllocation = nullptr;

    const uint32_t requiredPageCount = static_cast<uint32_t>(Align(byteSize, m_SystemPageSize) / m_SystemPageSize);
    const uint32_t newNextPageIndex = m_NextPageIndex + requiredPageCount;
    BIOME_ASSERT_MSG(requiredPageCount <= m_TotalPageCount, "ThreadHeapAllocator: Requested allocation exceeds allocator total byte size.");
    
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

bool ThreadHeapAllocator::WasAllocatedFromThisThreadInternal(void* pMemory)
{
    return 
        (reinterpret_cast<uintptr_t>(pMemory) >= m_MemoryPool) &&
        (reinterpret_cast<uintptr_t>(pMemory) < (m_MemoryPool + m_TotalPageCount * m_SystemPageSize));
}

bool ThreadHeapAllocator::ReleaseInternal(void *pMemory)
{
    if(!WasAllocatedFromThisThreadInternal(pMemory))
    {
        return false;
    }

    const uint32_t pageIndex = AddressToPageIndex(pMemory);
    FreeRange &range = m_pFreeRanges[m_FreeRangesCount++];
    range.m_Index = pageIndex;
    range.m_Count = m_pPages[pageIndex];

    return true;
}

size_t ThreadHeapAllocator::AllocationSizeInternal(void *pMemory)
{
    const uint32_t pageIndex = AddressToPageIndex(pMemory);
    return m_pPages[pageIndex] * m_SystemPageSize;
}

void ThreadHeapAllocator::DefragInternal()
{
    // #todo Implement allocator memory defrag
}

void ThreadHeapAllocator::CommitMorePages(uint32_t pageCount)
{
    const uint32_t reservedPageCount = m_TotalPageCount - m_CommittedPageCount;
    BIOME_ASSERT_MSG(pageCount <= reservedPageCount, 
        "ThreadHeapAllocator::CommitMorePages: Cannot commit requested number of pages. Not enough reserved memory");

    size_t newCommittedPageCount = std::min(std::max(pageCount, m_CommittedPageCount * 2), reservedPageCount);
    VirtualMemoryAllocator::Commit(PageIndexToAddress(m_CommittedPageCount), newCommittedPageCount * m_SystemPageSize);
}

uint32_t ThreadHeapAllocator::SearchFreePages(uint32_t pageCount)
{
    uint32_t rangeStartIndex = UINT32_MAX;

    const uint32_t rangeCount = m_FreeRangesCount;
    for (uint32_t i = 0; i < rangeCount; ++i)
    {
        FreeRange &range = m_pFreeRanges[i];
        if (range.m_Count >= pageCount)
        {
            rangeStartIndex = range.m_Index;
            m_pFreeRanges[i] = m_pFreeRanges[--m_FreeRangesCount];
            break;
        }
    }

    return rangeStartIndex;
}

void* ThreadHeapAllocator::PageIndexToAddress(uint32_t index)
{
    const uintptr_t offset = uintptr_t(index) * uintptr_t(m_SystemPageSize);
    return reinterpret_cast<void*>(m_MemoryPool + offset);
}

uint32_t ThreadHeapAllocator::AddressToPageIndex(void *pAddress)
{
    const uintptr_t address = reinterpret_cast<uintptr_t>(pAddress);
    const uintptr_t offset = address - m_MemoryPool;

    return static_cast<uint32_t>(offset / m_SystemPageSize);
}
