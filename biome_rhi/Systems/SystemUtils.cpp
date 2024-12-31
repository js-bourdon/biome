#include <pch.h>
#include "biome_rhi/Systems/SystemUtils.h"

using namespace biome::rhi;
using namespace biome::rhi::resources;

DescriptorHandle util::GetDescriptorHandle(DescriptorHeap& heap)
{
	const INT heapOffset = static_cast<INT>(heap.m_OffsetAllocator.AllocatePages(1));

	DescriptorHandle descriptorHdl = {};
	descriptorHdl.m_heapOffset = static_cast<uint32_t>(heapOffset);
	descriptorHdl.m_cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heap.m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), heapOffset);

	const D3D12_DESCRIPTOR_HEAP_DESC heapDesc = heap.m_pDescriptorHeap->GetDesc();
	if (heapDesc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	{
		descriptorHdl.m_gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(heap.m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), heapOffset);
	}

	return descriptorHdl;
}

void util::ReleaseDescriptorHandle(DescriptorHeap& heap, DescriptorHandle& handle)
{
	// TODO: Validate the given handle really comes from the given heap.

	const uint64_t handleOffset = handle.m_cpuHandle.ptr;
	const uint64_t startHandleOffset = heap.m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr;
	const uint64_t offset = handleOffset - startHandleOffset;
	BIOME_ASSERT(offset == handle.m_heapOffset);
	heap.m_OffsetAllocator.Release(offset);
}
