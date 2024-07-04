#pragma once

#include "Vector.h"
#include <cstdint>
#include <algorithm>
#include "biome_core/Core/Defines.h"

using namespace biome::data;

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
Vector<ValueType, CleanConstructDelete, AllocatorType>::Vector()
    : m_pData(nullptr)
    , m_size(0)
    , m_reservedSize(0)
{

}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
Vector<ValueType, CleanConstructDelete, AllocatorType>::Vector(uint32_t reservedSize)
    : m_pData(static_cast<ValueType*>(AllocatorType::Allocate(sizeof(ValueType) * reservedSize)))
    , m_size(0)
    , m_reservedSize(reservedSize)
{
    BIOME_ASSERT(reservedSize > 0);
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
Vector<ValueType, CleanConstructDelete, AllocatorType>::Vector(uint32_t reservedSize, uint32_t size)
    : m_pData(static_cast<ValueType*>(AllocatorType::Allocate(sizeof(ValueType)* reservedSize)))
    , m_size(size)
    , m_reservedSize(reservedSize)
{
    BIOME_ASSERT(reservedSize > 0);
    BIOME_ASSERT(m_reservedSize >= m_size);

    if constexpr (CleanConstructDelete)
    {
        for (size_t i = 0; i < m_size; ++i)
        {
            new(m_pData + i) ValueType();
        }
    }
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
Vector<ValueType, CleanConstructDelete, AllocatorType>::~Vector()
{
    Clear();
    AllocatorType::Release(m_pData);
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
uint32_t Vector<ValueType, CleanConstructDelete, AllocatorType>::Add(const ValueType& value)
{
    EnsureCapacity();
    m_pData[m_size] = value;

    const uint32_t index = m_size++;
    return index;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
void Vector<ValueType, CleanConstructDelete, AllocatorType>::Remove(uint32_t index)
{
    BIOME_ASSERT(index < m_size);
    const uint32_t startIndex = index + 1;

    for (uint32_t i = startIndex; i < m_size; ++i)
    {
        m_pData[i - 1] = std::move(m_pData[i]);
    }

    if constexpr (CleanConstructDelete)
    {
        if (startIndex == m_size)
        {
            m_pData[m_size - 1].~ValueType();
        }
    }

    --m_size;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
ValueType Vector<ValueType, CleanConstructDelete, AllocatorType>::PopBack()
{
    ValueType value = (*this)[m_size - 1];
    Remove(m_size - 1);
    return value;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
ValueType* Vector<ValueType, CleanConstructDelete, AllocatorType>::Data()
{
    return m_pData;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
ValueType* Vector<ValueType, CleanConstructDelete, AllocatorType>::Data() const
{
    return m_pData;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
void Vector<ValueType, CleanConstructDelete, AllocatorType>::Clear()
{
    if constexpr (CleanConstructDelete)
    {
        for (size_t i = 0; i < m_size; ++i)
        {
            m_pData[i].~ValueType();
        }
    }

    m_size = 0;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
ValueType& Vector<ValueType, CleanConstructDelete, AllocatorType>::operator[](size_t index)
{
    BIOME_ASSERT(index < m_size);
    return m_pData[index];
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
const ValueType& Vector<ValueType, CleanConstructDelete, AllocatorType>::operator[](size_t index) const
{
    BIOME_ASSERT(index < m_size);
    return m_pData[index];
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
template<typename ...T>
uint32_t Vector<ValueType, CleanConstructDelete, AllocatorType>::Emplace(T... args)
{
    EnsureCapacity();
    new (m_pData + m_size) ValueType(std::forward<T>(args)...);

    const uint32_t index = m_size++;
    return index;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
void Vector<ValueType, CleanConstructDelete, AllocatorType>::EnsureCapacity()
{
    if (m_size + 1 > m_reservedSize)
    {
        m_reservedSize = std::max(m_reservedSize, 2u) * 2;
        ValueType* pNewArray = static_cast<ValueType*>(AllocatorType::Allocate(m_reservedSize * sizeof(ValueType)));

        if constexpr (CleanConstructDelete)
        {
            for (uint32_t i = 0; i < m_reservedSize; ++i)
            {
                new (pNewArray + i) ValueType {};
            }
        }

        for (size_t i = 0; i < m_size; ++i)
        {
            pNewArray[i] = std::move(m_pData[i]);
        }

        AllocatorType::Release(m_pData);
        m_pData = pNewArray;
    }
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
ValueType* Vector<ValueType, CleanConstructDelete, AllocatorType>::begin()
{
    return m_pData;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
ValueType* Vector<ValueType, CleanConstructDelete, AllocatorType>::end()
{
    return m_pData + m_size;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
const ValueType* Vector<ValueType, CleanConstructDelete, AllocatorType>::cbegin() const
{
    return m_pData;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
const ValueType* Vector<ValueType, CleanConstructDelete, AllocatorType>::cend() const
{
    return m_pData + m_size;
}
