#pragma once

#include "Vector.h"
#include <cstdint>
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
    if constexpr (CleanConstructDelete)
    {
        for (size_t i = 0; i < m_size; ++i)
        {
            new(m_pData + i) ValueType();
        }
    }

    AllocatorType::Release(m_pData);
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
uint32_t Vector<ValueType, CleanConstructDelete, AllocatorType>::Add(ValueType& value)
{
    EnsureCapacity();
    m_pData[m_size] = value;

    const uint32_t index = m_size++;
    return index;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
ValueType& Vector<ValueType, CleanConstructDelete, AllocatorType>::Remove(uint32_t index)
{
    BIOME_ASSERT(index < m_size);
    const uint32_t startIndex = index + 1;

    for (uint32_t i = startIndex; i < m_size; ++i)
    {
        m_pData[i - 1] = m_pData[i];
    }
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
ValueType* Vector<ValueType, CleanConstructDelete, AllocatorType>::Data()
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
    new (m_pData + m_size) ValueType(args...);

    const uint32_t index = m_size++;
    return index;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
void Vector<ValueType, CleanConstructDelete, AllocatorType>::EnsureCapacity()
{
    if (m_size + 1 > m_reservedSize)
    {
        m_reservedSize *= 2;
        ValueType* pNewArray = static_cast<ValueType*>(AllocatorType::Allocate(m_reservedSize * sizeof(ValueType)));
        memcpy(pNewArray, m_pData, m_size * sizeof(ValueType));
        AllocatorType::Release(m_pData);
        m_pData = pNewArray;
    }
}
