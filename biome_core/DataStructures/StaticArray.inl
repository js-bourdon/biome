#pragma once

#include "StaticArray.h"
#include <cstdint>
#include "biome_core/Core/Defines.h"

using namespace biome::data;

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
StaticArray<ValueType, CleanConstructDelete, AllocatorType>::StaticArray()
    : m_pData(nullptr)
    , m_size(0)
{

}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
StaticArray<ValueType, CleanConstructDelete, AllocatorType>::StaticArray(size_t size)
    : m_pData(nullptr)
    , m_size(size)
{
    if(size > 0)
    {
        m_pData = static_cast<ValueType*>(AllocatorType::Allocate(sizeof(ValueType) * size));

        if constexpr (CleanConstructDelete)
        {
            for (size_t i = 0; i < m_size; ++i)
            {
                new(m_pData + i) ValueType();
            }
        }
    }
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
StaticArray<ValueType, CleanConstructDelete, AllocatorType>::StaticArray(StaticArray<ValueType, CleanConstructDelete, AllocatorType>&& other)
    : m_pData(other.m_pData)
    , m_size(other.m_size)
{
    other.m_pData = nullptr;
    other.m_size = 0;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
StaticArray<ValueType, CleanConstructDelete, AllocatorType>::~StaticArray()
{
    if(m_pData != nullptr)
    {
        if constexpr (CleanConstructDelete)
        {
            for (size_t i = 0; i < m_size; ++i)
            {
                m_pData[i].~ValueType();
            }
        }

        AllocatorType::Release(m_pData);
    }
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
ValueType* StaticArray<ValueType, CleanConstructDelete, AllocatorType>::Data()
{
    return m_pData;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
const ValueType* StaticArray<ValueType, CleanConstructDelete, AllocatorType>::Data() const
{
    return m_pData;
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
ValueType& StaticArray<ValueType, CleanConstructDelete, AllocatorType>::operator[](size_t index)
{
    BIOME_ASSERT(index < m_size);
    return m_pData[index];
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
const ValueType& StaticArray<ValueType, CleanConstructDelete, AllocatorType>::operator[](size_t index) const
{
    BIOME_ASSERT(index < m_size);
    return m_pData[index];
}

template<typename ValueType, bool CleanConstructDelete, typename AllocatorType>
void StaticArray<ValueType, CleanConstructDelete, AllocatorType>::operator=(StaticArray<ValueType, CleanConstructDelete, AllocatorType>&& other) noexcept
{
    this->~StaticArray();
    m_pData = other.m_pData;
    m_size = other.m_size;

    other.m_pData = nullptr;
    other.m_size = 0;
}
