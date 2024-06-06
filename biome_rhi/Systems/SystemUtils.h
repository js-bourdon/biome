#pragma once

#include "biome_rhi/Resources/Resources.h"

namespace biome::rhi
{
	namespace util
	{
		resources::DescriptorHandle GetDescriptorHandle(resources::DescriptorHeap& heap);
		void ReleaseDescriptorHandle(resources::DescriptorHeap& heap, resources::DescriptorHandle& handle);
	}
}