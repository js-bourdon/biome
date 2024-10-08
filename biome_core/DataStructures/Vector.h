#pragma once

#include <cstdint>
#include "biome_core/Memory/ThreadHeapAllocator.h"

using namespace biome::memory;

namespace biome
{
    namespace data
    {
        template<typename ValueType, bool CleanConstructDelete = false, typename AllocatorType = ThreadHeapAllocator>
        class Vector
        {
        public:
            Vector();
            Vector(uint32_t reservedSize);
            Vector(uint32_t reservedSize, uint32_t size);
            ~Vector();

            uint32_t            Add(const ValueType& value);
            void                Remove(uint32_t index);
            ValueType           PopBack();
            ValueType*          Data();
            ValueType*          Data() const;
            uint32_t            Size() const { return m_size; }
            void                Clear();

            ValueType&          operator[](size_t index);
            const ValueType&    operator[](size_t index) const;

            template<typename ...T>
            uint32_t            Emplace(T... args);

            ValueType*          begin();
            ValueType*          end();
            const ValueType*    cbegin() const;
            const ValueType*    cend() const;

        private:

            void                EnsureCapacity();

            ValueType*  m_pData;
            uint32_t    m_size;
            uint32_t    m_reservedSize;
        };
    }
}

#include "Vector.inl"
