#include <pch.h>
#include "FrameMemoryAllocator.h"

using namespace biome::memory;

FrameMemoryAllocator::FrameMemoryAllocator(size_t poolByteSize)
{
    Init(poolByteSize);
}

FrameMemoryAllocator::~FrameMemoryAllocator()
{
    if (m_MemoryPool)
    {
        FreeAlignedAlloc(m_MemoryPool);
    }
}

void FrameMemoryAllocator::Init(size_t poolByteSize)
{
    m_PoolByteSize = Align(poolByteSize, s_MaxAlignment);
    m_MemoryPool = static_cast<uint8_t*>(AlignedAlloc(m_PoolByteSize, s_MaxAlignment));
}

void* FrameMemoryAllocator::Allocate(size_t byteSize, size_t alignment)
{
    BIOME_ASSERT_MSG(alignment <= s_MaxAlignment, "FrameMemoryAllocator::Allocate: Maximum supported alignment exceeded.");

    size_t allocByteSize = Align(byteSize, alignment);
    size_t alignedOffset = Align(m_NextByte, alignment);
    m_NextByte = alignedOffset + allocByteSize;

    BIOME_ASSERT_MSG(m_NextByte <= m_PoolByteSize, "Over allocation for this frame");

    return m_MemoryPool + alignedOffset;
}

void FrameMemoryAllocator::Reset(MemoryBookmark bookmark)
{
    m_NextByte = bookmark;
}

void FrameMemoryAllocator::Reset()
{
    m_NextByte = 0;
}
