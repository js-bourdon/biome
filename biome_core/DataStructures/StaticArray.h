#pragma once

#include <cstdint>
#include "biome_core/Memory/ThreadHeapAllocator.h"

using namespace biome::memory;

namespace biome
{
    namespace data
    {
        constexpr bool NoCleanConstructDestruct = false;
        constexpr bool CleanConstructDestruct = true;

        // TODO: Have a version dealing with constructors / destructors of items.
        template<typename ValueType, bool CleanConstructDelete = false, typename AllocatorType = ThreadHeapAllocator>
        class StaticArray
        {
        public:

            StaticArray();
            StaticArray(size_t size);
            StaticArray(StaticArray&& other);
            StaticArray(const StaticArray& other) = delete;

            ~StaticArray();

            ValueType*          Data();
            const ValueType*    Data() const;
            size_t              Size() const { return m_size; }

            ValueType&          operator[](size_t index);
            const ValueType&    operator[](size_t index) const;
            void                operator=(StaticArray<ValueType, CleanConstructDelete, AllocatorType>&& other) noexcept;

        private:

            ValueType*  m_pData;
            size_t      m_size;
        };
    }
}

#include "StaticArray.inl"
