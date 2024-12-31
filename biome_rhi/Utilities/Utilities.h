#pragma once

namespace biome::rhi::descriptors { enum class Format; }
namespace biome::asset { enum class TextureFormat : int32_t; }

namespace biome::rhi::utils
{
    biome::rhi::descriptors::Format ConvertAssetTextureFormat(biome::asset::TextureFormat format);
}
