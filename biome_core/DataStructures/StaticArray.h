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

            template<typename... AddValueType, typename = std::enable_if_t<(... && std::is_same_v<ValueType, AddValueType>)>>
            StaticArray(AddValueType&&... values)
                : StaticArray(sizeof...(AddValueType))
            {
                SetValues(static_cast<size_t>(0), std::forward<AddValueType>(values)...);
            }

            template<typename... Args>
            StaticArray(const size_t size, Args&&... args)
                : StaticArray(size)
            {
                for (size_t index = 0; index < size; ++index)
                {
                    new (m_pData + index) ValueType(std::forward<Args>(args)...);
                }
            }

            ~StaticArray();

            ValueType*          Data();
            const ValueType*    Data() const;
            size_t              Size() const { return m_size; }

            ValueType&          operator[](size_t index);
            const ValueType&    operator[](size_t index) const;
            void                operator=(StaticArray<ValueType, CleanConstructDelete, AllocatorType>&& other) noexcept;

            ValueType*          begin();
            ValueType*          end();
            const ValueType*    cbegin() const;
            const ValueType*    cend() const;

        private:

            template<typename... AddValueType, typename = std::enable_if_t<(... && std::is_same_v<ValueType, AddValueType>)>>
            void SetValues(const size_t index, ValueType&& value, AddValueType&&... values)
            {
                m_pData[index] = std::forward<ValueType>(value);
                SetValues(index + 1, std::forward<AddValueType>(values)...);
            }

            void SetValues(const size_t index, ValueType&& value)
            {
                m_pData[index] = std::forward<ValueType>(value);
            }

            ValueType*  m_pData;
            size_t      m_size;
        };
    }
}

#include "StaticArray.inl"
