#include <pch.h>
#include "biome_rhi/Utilities/Utilities.h"
#include "biome_rhi/Descriptors/Formats.h"
#include "biome_core/Assets/Texture.h"

using namespace biome::asset;
using namespace biome::rhi::descriptors;

biome::rhi::descriptors::Format biome::rhi::utils::ConvertAssetTextureFormat(biome::asset::TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::R_Float:
        return Format::R32_FLOAT;
    case TextureFormat::RG_Float:
        return Format::R32G32_FLOAT;
    case TextureFormat::RBG_Float:
        return Format::R32G32B32_FLOAT;
    case TextureFormat::RBGA_Float:
        return Format::R32G32B32A32_FLOAT;
    case TextureFormat::BC1:
        return Format::BC1_UNORM;
    case TextureFormat::BC2:
        return Format::BC2_UNORM;
    case TextureFormat::BC3:
        return Format::BC3_UNORM;
    default:
        return Format::Unknown;
    }
}