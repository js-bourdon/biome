#include <pch.h>
#include "RingBuffer.h"
#include "Memory/ThreadHeapAllocator.h"

using namespace biome::memory;

static uintptr_t AllocateBuffer(size_t byteSize)
{
    // Aligned to page size by default
    return reinterpret_cast<uintptr_t>(ThreadHeapAllocator::Allocate(byteSize));
}

RingBuffer::RingBuffer(size_t byteSize)
    : m_RingStart(AllocateBuffer(byteSize))
    , m_RingEnd(m_RingStart + byteSize)
    , m_NextAddr(m_RingStart)
#ifdef _DEBUG
    , m_FrameStart(m_RingStart)
#endif
{

}

RingBuffer::~RingBuffer()
{
    ThreadHeapAllocator::Release(reinterpret_cast<void*>(m_RingStart));
}

void* RingBuffer::Allocate(size_t byteSize, size_t alignment)
{
    BIOME_ASSERT_MSG(Align(byteSize, alignment) <= (m_RingEnd - m_RingStart), 
        "RingBuffer::Allocate: Requested aligned allocation larger than total buffer size");

    uintptr_t nextAddr = biome::memory::Align(m_NextAddr, alignment);
    uintptr_t endAddr = nextAddr + byteSize;
    nextAddr = endAddr >= m_RingEnd ? m_RingStart : nextAddr;
    m_NextAddr = nextAddr + byteSize;

    BIOME_ASSERT_MSG(m_NextAddr <= m_FrameStart || nextAddr < m_NextAddr, "RingBuffer::Allocate: Overallocation for this frame");

    return reinterpret_cast<void*>(nextAddr);
}

void RingBuffer::EndFrame()
{
#ifdef _DEBUG
    m_FrameStart = m_NextAddr;
#endif
}
