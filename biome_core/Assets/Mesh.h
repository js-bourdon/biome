#pragma once

#include <stdint.h>

namespace biome
{
    namespace asset
    {
        enum class VertexAttribute : uint32_t
        {
            Position,
            Color,
            Normal,
            Tangent,
            UV,
            Count
        };

        static constexpr const char* cppVertexAttributeSemantics[] =
        {
            "POSITION",
            "COLOR_0",
            "NORMAL",
            "TANGENT",
            "TEXCOORD_0"
        };

        static_assert(BIOME_ARRAY_SIZE(cppVertexAttributeSemantics) == static_cast<size_t>(VertexAttribute::Count));

        struct BufferView
        {
            uint64_t m_byteSize { 0 };
            uint64_t m_byteOffset { 0 };
            uint64_t m_byteStride { 0 };
        };

        struct VertexStream : BufferView
        {
            VertexAttribute m_attribute {};
        };

        struct SubMesh
        {
            BufferView      m_indexBuffer {};
            uint32_t        m_textureIndex { std::numeric_limits<uint32_t>::max() };
            uint32_t        m_streamCount { 0 };
            VertexStream    m_streams[1] {}; // Needs to be at the end to adapt to allocation size.
        };

        struct Mesh
        {
            uint64_t m_subMeshCount { 0 };
            SubMesh m_subMeshes[1] {}; // Needs to be at the end to adapt to allocation size.
        };

        struct MeshAsset
        {
            uint32_t m_meshletOffset { 0 };
            uint32_t m_meshletCount { 0 };
        };
    }
}