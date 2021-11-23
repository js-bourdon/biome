#pragma once

#include <cstdint>
#include "biome_core/Memory/ThreadHeapAllocator.h"

using namespace biome::memory;

namespace biome
{
    namespace data
    {
        template<typename ValueType, typename AllocatorType = ThreadHeapAllocator>
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
            void                operator=(StaticArray<ValueType, AllocatorType>&& other);

        private:

            ValueType*  m_pData;
            size_t      m_size;
        };
    }
}

#include "StaticArray.inl"
