#include <pch.h>
#include "biome_core/Memory/SubAllocator.h"
#include "biome_core/Memory/ThreadHeapAllocator.h"

using namespace biome::memory;

SubAllocator::~SubAllocator()
{
    if (m_pAllocation)
    {
        ThreadHeapAllocator::Release(m_pAllocation);
    }
}
