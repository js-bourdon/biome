#pragma once

#include <cstdint>
#include <stdio.h>
#include <limits>
#include "asset_assembler/rapidjson/fwd.h"
#include "biome_core/DataStructures/Vector.h"
#include "biome_core/Assets/Mesh.h"

using namespace rapidjson;
using namespace biome::data;
using namespace biome::asset;

namespace asset_assembler
{
    namespace database
    {
        /*
        enum class ComponentType
        {
            Unknown = -1,
            Scalar_Float,
            Scalar_Byte,
            Scalar_Short,
            Scalar_Int,
            Vec2,
            Vec3,
            Vec4,
            Matrix2x2,
            Matrix3x3,
            Matrix4x4,
            Count
        };
        */

        class AssetDatabaseBuilder
        {
        public:

            AssetDatabaseBuilder() = default;
            ~AssetDatabaseBuilder() = default;

            bool BuildDatabase(const char *pSrcPath, const char *pDstPath);

        private:

            struct PackedBufferMeta
            {
                PackedBufferMeta(uint64_t byteOffset, uint64_t byteSize) : m_byteOffset(byteOffset), m_byteSize(byteSize) {}
                uint64_t m_byteOffset;
                uint64_t m_byteSize;
            };

            static constexpr uint32_t   cInvalidIndex = std::numeric_limits<uint32_t>::max();
            static constexpr const char cpBuffersBinFileName[] = "Buffers.bin";
            static constexpr const char cpTexturesBinFileName[] = "Textures.bin";

            template<typename T>
            static bool WriteData(const T& value, FILE* pFile);

            bool        PackData(const Document& json, const char* pSrcRootPath, const char* pDestRootPath);
            bool        PackTextures(const Document &json, const char *pSrcRootPath, const char *pDestRootPath);
            bool        PackBuffers(const Document &json, const char *pSrcRootPath, const char *pDestRootPath);

            bool        InsertMeta(const Document& json, FILE* pDBFile);
            bool        InsertTexturesMeta(const Document& json, FILE* pDBFile);
            bool        InsertMeshesMeta(const Document& json, FILE* pDBFile);

            uint32_t    GetTextureIndex(const Document& json, SizeType materialIndex);
            void        GetBufferView(const Document& json, SizeType accessorIndex, BufferView& oView);
            uint32_t    GetSupportedAttributeCount(const Value& attributes);
            int64_t     CompressTexture(const char* pSrcFilePath, FILE* pDestFile);

        private:

            Vector<PackedBufferMeta> m_texturesMeta { 100 };
            Vector<PackedBufferMeta> m_buffersMeta { 100 };
        };
    }
}