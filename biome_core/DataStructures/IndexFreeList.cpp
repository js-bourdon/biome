#include <pch.h>
#include "IndexFreeList.h"
#include <assert.h>

using namespace biome;
using namespace biome::data;

IndexFreeList::IndexFreeList(IndexFreeList&& other) noexcept
{
    m_Items = other.m_Items;
    m_Capacity = other.m_Capacity;
    m_Count = other.m_Count;
    m_FreeListIndex = other.m_FreeListIndex;

    other.m_Items = nullptr;
}

IndexFreeList& IndexFreeList::operator=(IndexFreeList &&other) noexcept
{
    m_Items = other.m_Items;
    m_Capacity = other.m_Capacity;
    m_Count = other.m_Count;
    m_FreeListIndex = other.m_FreeListIndex;

    other.m_Items = nullptr;

    return *this;
}

void IndexFreeList::Init(uint32_t capacity)
{
    m_Capacity = capacity;
    m_Items = static_cast<Item*>(biome::memory::AlignedAlloc(capacity, alignof(Item)));

    for (uint32_t i = 0; i < m_Capacity; ++i)
    {
        m_Items[i].m_Next = i + 1;
    }
}

IndexFreeList::~IndexFreeList()
{
    if (m_Items)
    {
        biome::memory::FreeAlignedAlloc(m_Items);
    }
}

Handle IndexFreeList::Add(uint32_t index)
{
    BIOME_ASSERT_MSG(m_FreeListIndex < m_Capacity && m_Count < m_Capacity, "Free list is full");

    ++m_Count;

    Handle handle { m_FreeListIndex };
    Item &item = m_Items[m_FreeListIndex];
    m_FreeListIndex = item.m_Next;
    item.m_Handle.SetValue(index);

#if VALIDATE_HANDLES
    handle.SetGeneration(item.m_Handle.GetGeneration());
#endif

    return handle;
}

uint32_t IndexFreeList::Remove(const Handle &handle)
{
    BIOME_ASSERT_MSG(ValidateHandle(handle), "IndexFreeList::Remove: Invalid handle. Most probably already removed");

    --m_Count;

    Item &item = m_Items[handle.GetValue()];
    item.m_Next = m_FreeListIndex;
    m_FreeListIndex = static_cast<uint32_t>(handle);

#if VALIDATE_HANDLES
    item.m_Handle.IncrementGeneration(); // Bump generation so any remaining handle copy is invalidated
#endif

    return static_cast<uint32_t>(item.m_Handle.GetValue());
}

uint32_t IndexFreeList::Get(const Handle &handle) const
{
    BIOME_ASSERT_MSG(ValidateHandle(handle), "IndexFreeList::Get: Invalid handle.");
    Item &item = m_Items[handle];
    return static_cast<uint32_t>(item.m_Handle);
}

void IndexFreeList::Update(const Handle &handle, uint32_t index)
{
    BIOME_ASSERT_MSG(ValidateHandle(handle), "IndexFreeList::Update: Invalid handle.");
    Item &item = m_Items[handle];
    item.m_Handle.SetValue(index);
}

bool IndexFreeList::ValidateHandle(const Handle& handle) const
{
    return (handle < m_Capacity) 
#if VALIDATE_HANDLES
        && (handle.GetGeneration() == m_Items[handle].m_Handle.GetGeneration())
#endif
        ;
}
