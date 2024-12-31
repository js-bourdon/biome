#pragma once

#include <malloc.h>
#include <new>
#include "biome_core/Core/Utilities.h"

// Global allocation operator overrides
void* operator new(std::size_t count);
void* operator new[](std::size_t count);
void operator delete(void *ptr);
void operator delete[](void *ptr);

#ifdef __cpp_aligned_new
    void* operator new(std::size_t count, std::align_val_t al);
    void* operator new[](std::size_t count, std::align_val_t al);
    void operator delete(void *ptr, std::align_val_t al);
    void operator delete[](void *ptr, std::align_val_t al);
#endif


namespace biome
{
    namespace memory
    {
        void*   AlignedAlloc(size_t size, size_t alignment);
        void*   AlignedRealloc(void *pMemory, size_t newSize, size_t alignment);
        void    FreeAlignedAlloc(void *pAlloc);

        inline void* StackAlloc(size_t size)
        {
            return _malloca(size);
        }

        template<typename T> concept UnsignedType = std::is_unsigned_v<T>;
        template<UnsignedType T0, UnsignedType T1>
        inline constexpr utils::LargestType<T0, T1> Align(T0 size, T1 alignment)
        {
            const utils::LargestType<T0, T1> mask = alignment - 1;
            return (size + mask) & ~mask;
        }
    }
}