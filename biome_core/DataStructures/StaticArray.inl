#pragma once

#include "StaticArray.h"
#include <cstdint>
#include "biome_core/Core/Defines.h"

using namespace biome::data;

template<typename ValueType, typename AllocatorType>
StaticArray<ValueType, AllocatorType>::StaticArray()
    : m_pData(nullptr)
    , m_size(0)
{

}

template<typename ValueType, typename AllocatorType>
StaticArray<ValueType, AllocatorType>::StaticArray(size_t size)
    : m_pData(nullptr)
    , m_size(size)
{
    if(size > 0)
    {
        m_pData = static_cast<ValueType*>(AllocatorType::Allocate(sizeof(ValueType) * size));
    }
}

template<typename ValueType, typename AllocatorType>
StaticArray<ValueType, AllocatorType>::StaticArray(StaticArray<ValueType, AllocatorType>&& other)
    : m_pData(other.m_pData)
    , m_size(other.m_size)
{
    other.m_pData = nullptr;
    other.m_size = 0;
}

template<typename ValueType, typename AllocatorType>
StaticArray<ValueType, AllocatorType>::~StaticArray()
{
    if(m_pData != nullptr)
    {
        AllocatorType::Release(m_pData);
    }
}

template<typename ValueType, typename AllocatorType>
ValueType* StaticArray<ValueType, AllocatorType>::Data()
{
    return m_pData;
}

template<typename ValueType, typename AllocatorType>
const ValueType* StaticArray<ValueType, AllocatorType>::Data() const
{
    return m_pData;
}

template<typename ValueType, typename AllocatorType>
ValueType& StaticArray<ValueType, AllocatorType>::operator[](size_t index)
{
    BIOME_ASSERT(index < m_size);
    return m_pData[index];
}

template<typename ValueType, typename AllocatorType>
const ValueType& StaticArray<ValueType, AllocatorType>::operator[](size_t index) const
{
    BIOME_ASSERT(index < m_size);
    return m_pData[index];
}

template<typename ValueType, typename AllocatorType>
void StaticArray<ValueType, AllocatorType>::operator=(StaticArray<ValueType, AllocatorType>&& other)
{
    m_pData = other.m_pData;
    m_size = other.m_size;

    other.m_pData = nullptr;
    other.m_size = 0;
}
