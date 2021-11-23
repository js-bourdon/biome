#pragma once

#include <stdint.h>
#include "biome_core/Memory/Memory.h"
#include "biome_core/Core/Defines.h"

namespace biome
{
    namespace memory
    {
        class ThreadHeapAllocator
        {
        public:

            ThreadHeapAllocator() = default;
            ThreadHeapAllocator(const ThreadHeapAllocator&) = delete;
            ThreadHeapAllocator(ThreadHeapAllocator&&) = delete;
            ThreadHeapAllocator& operator=(const ThreadHeapAllocator&) = delete;
            ThreadHeapAllocator& operator=(ThreadHeapAllocator&&) = delete;
            ~ThreadHeapAllocator();

            [[nodiscard]] 
            static bool     Initialize(size_t heapByteSize, size_t initialCommitByteSize);
            [[nodiscard]]
            static bool     IsInitialized();
            static void     Shutdown();
            static void*    Allocate(size_t byteSize);
            static bool     WasAllocatedFromThisThread(void *pMemory);
            static bool     Release(void *pMemory);
            static size_t   AllocationSize(void *pMemory);
            static void     Defrag();

        private:

            static constexpr size_t DefaultHeapByteSize = GiB(1);
            static constexpr size_t DefaultInitialCommitByteSize = MiB(100);

            struct FreeRange
            {
                uint32_t m_Index;
                uint32_t m_Count;
            };

            static ThreadHeapAllocator* CreateAllocator(size_t heapByteSize, size_t initialCommitByteSize);

            void*       AllocateInternal(size_t byteSize);
            bool        WasAllocatedFromThisThreadInternal(void* pMemory);
            bool        ReleaseInternal(void *pMemory);
            size_t      AllocationSizeInternal(void *pMemory);
            void        DefragInternal();

            void        CommitMorePages(uint32_t pageCount);
            uint32_t    SearchFreePages(uint32_t pageCount);
            void*       PageIndexToAddress(uint32_t index);
            uint32_t    AddressToPageIndex(void *pAddress);

            thread_local static ThreadHeapAllocator* s_pAllocator;

            uintptr_t   m_MemoryPool { 0 };
            FreeRange*  m_pFreeRanges { nullptr };
            uint32_t*   m_pPages { nullptr };
            size_t      m_SystemPageSize { 0 };
            uint32_t    m_TotalPageCount { 0 };
            uint32_t    m_CommittedPageCount { 0 };
            uint32_t    m_NextPageIndex { 0 };
            uint32_t    m_FreeRangesCount { 0 };
        };
    }
}