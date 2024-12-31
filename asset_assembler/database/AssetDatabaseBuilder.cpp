#include <pch.h>
#include "AssetDatabaseBuilder.h"
#include "biome_core/Memory/ThreadHeapAllocator.h"
#include "biome_core/Memory/ThreadHeapSmartPointer.h"
#include "biome_core/FileSystem/FileSystem.h"
#include "biome_core/Assets/AssetDatabase.h"
#include "biome_core/Assets/Texture.h"
#include "rapidjson/document.h"
#include "stb/stb_image.h"
#include "stb/stb_dxt.h"

using namespace asset_assembler::database;
using namespace biome;
using namespace biome::asset;
using namespace biome::memory;
using namespace biome::filesystem;

template<typename T>
static bool AssetDatabaseBuilder::WriteData(const T& value, FILE* pFile)
{
    return fwrite(&value, sizeof(T), 1, pFile) == 1;
}

bool AssetDatabaseBuilder::BuildDatabase(const char* pSrcPath, const char* pDstPath)
{
    m_buffersMeta.Clear();
    m_texturesMeta.Clear();

    size_t jsonContentSize;
    char* pJsonContent = reinterpret_cast<char*>(ReadFileContent<ThreadHeapAllocator>(pSrcPath, jsonContentSize));

    if (!pJsonContent)
    {
        return false;
    }

    Document json;
    json.Parse(pJsonContent);

    // Pack data
    {
        ThreadHeapAllocator::Release(pJsonContent);

        const char* pDstRootPathEnd = strrchr(pDstPath, '/');
        const char* pSrcRootPathEnd = strrchr(pSrcPath, '/');

        if (!pDstRootPathEnd || !pSrcRootPathEnd)
        {
            return false;
        }

        size_t dstRootFolderStrLen =
            static_cast<size_t>(reinterpret_cast<uintptr_t>(pDstRootPathEnd) - reinterpret_cast<uintptr_t>(pDstPath)) + 1;
        size_t srcRootFolderStrLen =
            static_cast<size_t>(reinterpret_cast<uintptr_t>(pSrcRootPathEnd) - reinterpret_cast<uintptr_t>(pSrcPath)) + 1;

        char* pDstRootPath = static_cast<char*>(biome::memory::StackAlloc(dstRootFolderStrLen + 1));
        char* pSrcRootPath = static_cast<char*>(biome::memory::StackAlloc(srcRootFolderStrLen + 1));

        pDstRootPath[dstRootFolderStrLen] = 0;
        pSrcRootPath[srcRootFolderStrLen] = 0;

        memcpy(pDstRootPath, pDstPath, dstRootFolderStrLen);
        memcpy(pSrcRootPath, pSrcPath, srcRootFolderStrLen);

        if (!PackData(json, pSrcRootPath, pDstRootPath))
        {
            return false;
        }
    }

    // Write metadata
    {
        FILE* pDBFile = nullptr;
        if (fopen_s(&pDBFile, pDstPath, "wb") != 0)
        {
            return false;
        }

        FileHandleRAII fileRAII(pDBFile);

        AssetDatabaseHeader header {};
        strcpy_s(header.m_pPackedBuffersFileName, cpBuffersBinFileName);
        strcpy_s(header.m_pPackedTexturesFileName, cpTexturesBinFileName);

        header.m_meshCount = m_buffersMeta.Size();
        header.m_textureCount = m_texturesMeta.Size();

        if (!WriteData(header, pDBFile))
        {
            return false;
        }

        if (!InsertMeta(json, pDBFile))
        {
            return false;
        }
    }

    return true;
}

bool AssetDatabaseBuilder::PackData(
    const Document& json,
    const char* pSrcRootPath,
    const char* pDestRootPath)
{
    return
        PackTextures(json, pSrcRootPath, pDestRootPath) &&
        PackBuffers(json, pSrcRootPath, pDestRootPath);
}

bool AssetDatabaseBuilder::PackTextures(const Document &json, const char *pSrcRootPath, const char *pDestRootPath)
{
    static constexpr const char s_pTexturesBinFileName[] = "Textures.bin";
    static constexpr const char s_pImgProperty[] = "images";
    static constexpr const char s_pUriProperty[] = "uri";

    if (json.HasMember(s_pImgProperty) && json[s_pImgProperty].IsArray())
    {
        const Value &images = json[s_pImgProperty];
        SizeType imageCount = images.Size();

        if (imageCount > 0)
        {
            str_smart_ptr pDestFilePath = biome::filesystem::AppendPaths(pDestRootPath, s_pTexturesBinFileName);
            FILE *pDestFile = nullptr;

            if (fopen_s(&pDestFile, pDestFilePath, "wb") != 0)
            {
                return false;
            }

            FileHandleRAII fileRAII(pDestFile);

            int64_t currentByteOffset = 0;

            for (SizeType i = 0; i < imageCount; ++i)
            {
                const Value &img = images[i];
                if (img.HasMember(s_pUriProperty) && img[s_pUriProperty].IsString())
                {
                    const Value &uri = img[s_pUriProperty];
                    const char *pTextureUri = uri.GetString();

                    str_smart_ptr pSrcFilePath = biome::filesystem::AppendPaths(pSrcRootPath, pTextureUri);
                    const TextureInfo textureInfo = CompressTexture(pSrcFilePath, pDestFile);
                    if (textureInfo.m_byteSize == 0)
                    {
                        return false;
                    }

                    m_texturesMeta.Emplace(currentByteOffset, textureInfo.m_byteSize, textureInfo.m_pixelWidth, textureInfo.m_pixelHeight);
                    currentByteOffset += textureInfo.m_byteSize;
                }
            }
        }
    }

    return true;
}

bool AssetDatabaseBuilder::PackBuffers(const Document &json, const char *pSrcRootPath, const char *pDestRootPath)
{
    static constexpr const char cpBuffersBinFileName[] = "Buffers.bin";
    static constexpr const char cpBuffersProperty[] = "buffers";
    static constexpr const char cpUriProperty[] = "uri";

    if (json.HasMember(cpBuffersProperty) && json[cpBuffersProperty].IsArray())
    {
        const Value &buffers = json[cpBuffersProperty];
        const SizeType bufferCount = buffers.Size();

        if (bufferCount > 0)
        {
            str_smart_ptr destFilePath = biome::filesystem::AppendPaths(pDestRootPath, cpBuffersBinFileName);

            FILE *pDestFile = nullptr;
            if (fopen_s(&pDestFile, destFilePath, "wb") != 0)
            {
                return false;
            }

            FileHandleRAII fileRAII(pDestFile);

            uint64_t currentByteOffset = 0;

            for (SizeType i = 0; i < bufferCount; ++i)
            {
                const Value &buffer = buffers[i];
                if (buffer.HasMember(cpUriProperty) && buffer[cpUriProperty].IsString())
                {
                    const Value &uri = buffer[cpUriProperty];
                    const char *pBufferUri = uri.GetString();

                    size_t fileSize = 0;
                    str_smart_ptr pSrcFilePath = biome::filesystem::AppendPaths(pSrcRootPath, pBufferUri);
                    uint8_t *pData = ReadFileContent<ThreadHeapAllocator>(pSrcFilePath, fileSize);

                    if (fileSize == 0 || fwrite(pData, sizeof(uint8_t), fileSize, pDestFile) != fileSize)
                    {
                        return false;
                    }

                    m_buffersMeta.Emplace(currentByteOffset, fileSize);

                    currentByteOffset += static_cast<uint64_t>(fileSize);

                    ThreadHeapAllocator::Release(pData);
                }
            }
        }
    }

    return true;
}

bool AssetDatabaseBuilder::InsertMeta(const Document& json, FILE* pDBFile)
{
    return
        InsertTexturesMeta(json, pDBFile) &&
        InsertMeshesMeta(json, pDBFile);
}

bool AssetDatabaseBuilder::InsertTexturesMeta(const Document& json, FILE* pDBFile)
{
    uint32_t textureCount = m_texturesMeta.Size();
    for (uint32_t i = 0; i < textureCount; ++i)
    {
        const PackedTextureMeta& meta = m_texturesMeta[i];
        Texture texture { meta.m_byteSize, meta.m_byteOffset, meta.m_pixelWidth, meta.m_pixelHeight, TextureFormat::BC3 };

        if (!WriteData(texture, pDBFile))
        {
            return false;
        }
    }

    return true;
}

bool AssetDatabaseBuilder::InsertMeshesMeta(const Document& json, FILE* pDBFile)
{
    static constexpr const char cpMeshesProperty[] = "meshes";
    static constexpr const char cpSubMeshesProperty[] = "primitives";
    static constexpr const char cpIndicesProperty[] = "indices";
    static constexpr const char cpMaterialProperty[] = "material";
    static constexpr const char cpAttributesProperty[] = "attributes";

    if (json.HasMember(cpMeshesProperty) && json[cpMeshesProperty].IsArray())
    {
        const Value& meshes = json[cpMeshesProperty];
        const SizeType meshCount = meshes.Size();

        // Write every mesh
        for (SizeType meshIndex = 0; meshIndex < meshCount; ++meshIndex)
        {
            const Value& mesh = meshes[meshIndex];

            if (mesh.HasMember(cpSubMeshesProperty) && mesh[cpSubMeshesProperty].IsArray())
            {
                const Value& subMeshes = mesh[cpSubMeshesProperty];
                const SizeType subMeshCount = subMeshes.Size();

                // Writing a Mesh in two steps, count then array of sub meshes.
                if (!WriteData(static_cast<uint64_t>(subMeshCount), pDBFile))
                {
                    return false;
                }

                // Write sub meshes for current mesh
                for (SizeType subMeshIndex = 0; subMeshIndex < subMeshCount; ++subMeshIndex)
                {
                    const Value& subMesh = subMeshes[subMeshIndex];

                    if (
                        subMesh.HasMember(cpIndicesProperty) && subMesh[cpIndicesProperty].IsInt() &&
                        subMesh.HasMember(cpMaterialProperty) && subMesh[cpMaterialProperty].IsInt() &&
                        subMesh.HasMember(cpAttributesProperty) && subMesh[cpAttributesProperty].IsObject())
                    {
                        SubMeshHeader subMeshHeader {};

                        const SizeType indexBufferIndex = subMesh[cpIndicesProperty].GetInt();
                        const SizeType materialIndex = subMesh[cpMaterialProperty].GetInt();
                        const Value& attributes = subMesh[cpAttributesProperty];

                        subMeshHeader.m_textureIndex = GetTextureIndex(json, materialIndex);
                        subMeshHeader.m_streamCount = GetSupportedAttributeCount(attributes);
                        GetBufferView(json, indexBufferIndex, subMeshHeader.m_indexBuffer);
                        
                        if (!WriteData(subMeshHeader, pDBFile))
                        {
                            return false;
                        }

                        // Write streams for current sub mesh
                        for (size_t i = 0; i < BIOME_ARRAY_SIZE(cppVertexAttributeSemantics); ++i)
                        {
                            const char* pAttributeSemantic = cppVertexAttributeSemantics[i];
                            if (attributes.HasMember(pAttributeSemantic) && attributes[pAttributeSemantic].IsInt())
                            {
                                const SizeType accessorIndex = attributes[pAttributeSemantic].GetInt();
                                VertexStream stream {};
                                stream.m_attribute = static_cast<VertexAttribute>(i);
                                GetBufferView(json, accessorIndex, stream);

                                if (!WriteData(stream, pDBFile))
                                {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

uint32_t AssetDatabaseBuilder::GetSupportedAttributeCount(const Value& attributes)
{
    BIOME_ASSERT(attributes.IsObject());

    uint32_t count = 0;

    for (size_t i = 0; i < BIOME_ARRAY_SIZE(cppVertexAttributeSemantics); ++i)
    {
        const char* pAttributeSemantic = cppVertexAttributeSemantics[i];
        if (attributes.HasMember(pAttributeSemantic) && attributes[pAttributeSemantic].IsInt())
        {
            ++count;
        }
    }

    return count;
}

uint32_t AssetDatabaseBuilder::GetTextureIndex(const Document& json, SizeType materialIndex)
{
    static constexpr const char s_pTexturesProperty[] = "textures";
    static constexpr const char s_pMaterialsProperty[] = "materials";
    static constexpr const char s_pPBRProperty[] = "pbrMetallicRoughness";
    static constexpr const char s_pBaseTextureProperty[] = "baseColorTexture";
    static constexpr const char s_pIndexProperty[] = "index";
    static constexpr const char s_pSourceProperty[] = "source";

    uint32_t index = cInvalidIndex;

    if (json.HasMember(s_pMaterialsProperty) && json[s_pMaterialsProperty].IsArray())
    {
        const Value& materials = json[s_pMaterialsProperty];
        const SizeType materialCount = materials.Size();

        //for (SizeType i = 0; i < materialCount; ++i)
        if (materialIndex < materialCount)
        {
            const Value& material = materials[materialIndex];
            if (material.HasMember(s_pPBRProperty) && material[s_pPBRProperty].IsObject())
            {
                const Value& pbr = material[s_pPBRProperty];
                if (pbr.HasMember(s_pBaseTextureProperty) && pbr[s_pBaseTextureProperty].IsObject())
                {
                    const Value& baseTexture = pbr[s_pBaseTextureProperty];
                    if (baseTexture.HasMember(s_pIndexProperty) && baseTexture[s_pIndexProperty].IsInt())
                    {
                        const Value& indexProperty = baseTexture[s_pIndexProperty];
                        SizeType textureIndex = indexProperty.GetInt();

                        if (json.HasMember(s_pTexturesProperty) && json[s_pTexturesProperty].IsArray())
                        {
                            const Value& textures = json[s_pTexturesProperty];
                            const SizeType textureCount = textures.Size();
                            if (textureIndex < textureCount)
                            {
                                const Value& texture = textures[textureIndex];
                                if (texture.HasMember(s_pSourceProperty) && texture[s_pSourceProperty].IsInt())
                                {
                                    index = texture[s_pSourceProperty].GetInt();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return index;
}

void AssetDatabaseBuilder::GetBufferView(const Document& json, SizeType accessorIndex, BufferView& oView)
{
    static constexpr const char s_pAccessorsProperty[] = "accessors";
    static constexpr const char s_pBufferViewsProperty[] = "bufferViews";
    static constexpr const char s_pBufferViewProperty[] = "bufferView";
    static constexpr const char s_pByteOffsetProperty[] = "byteOffset";
    static constexpr const char s_pByteLengthProperty[] = "byteLength";
    static constexpr const char s_pByteStrideProperty[] = "byteStride";

    if (json.HasMember(s_pAccessorsProperty) && json[s_pAccessorsProperty].IsArray() &&
        json.HasMember(s_pBufferViewsProperty) && json[s_pBufferViewsProperty].IsArray())
    {
        const Value& accessors = json[s_pAccessorsProperty];
        const Value& bufferViews = json[s_pBufferViewsProperty];
        const SizeType accessorCount = accessors.Size();

        BIOME_ASSERT(accessorIndex < accessorCount);

        const Value& accessor = accessors[accessorIndex];
        if (accessor.HasMember(s_pBufferViewProperty) && accessor[s_pBufferViewProperty].IsInt())
        {
            const int bufferViewIndex = accessor[s_pBufferViewProperty].GetInt();
            const Value& bufferView = bufferViews[bufferViewIndex];

            if (bufferView.HasMember(s_pByteLengthProperty) && bufferView[s_pByteLengthProperty].IsInt())
            {
                oView.m_byteSize = bufferView[s_pByteLengthProperty].GetInt();

                uint64_t accessorByteOffset = 0;
                uint64_t bufferViewByteOffset = 0;
                uint64_t bufferViewByteStride = 1;

                if (accessor.HasMember(s_pByteOffsetProperty) && accessor[s_pByteOffsetProperty].IsInt())
                {
                    accessorByteOffset = accessor[s_pByteOffsetProperty].GetInt();
                }

                if (bufferView.HasMember(s_pByteOffsetProperty) && bufferView[s_pByteOffsetProperty].IsInt())
                {
                    bufferViewByteOffset = bufferView[s_pByteOffsetProperty].GetInt();
                }

                if (bufferView.HasMember(s_pByteStrideProperty) && bufferView[s_pByteStrideProperty].IsInt())
                {
                    bufferViewByteStride = bufferView[s_pByteStrideProperty].GetInt();
                }

                oView.m_byteOffset = accessorByteOffset + bufferViewByteOffset;
                oView.m_byteStride = bufferViewByteStride;
            }
        }
    }
}

AssetDatabaseBuilder::TextureInfo AssetDatabaseBuilder::CompressTexture(const char* pSrcFilePath, FILE* pDestFile)
{
    // TODO: Generate MIP chain if not already generated

    // Load images with stb_image
    // https://github.com/nothings/stb/blob/master/stb_image.h

    int width, height, componentCount;
    constexpr int mode = 0; // Normal mode
    const unsigned char* fileContent = stbi_load(pSrcFilePath, &width, &height, &componentCount, mode);

    BIOME_ASSERT(componentCount == 3 || componentCount == 4);

    // Block compress using stb_dxt
    // https://github.com/nothings/stb/blob/master/stb_dxt.h

    BIOME_ASSERT(width % 4 == 0);
    BIOME_ASSERT(height % 4 == 0);

    const uint32_t pixelWidth = static_cast<uint32_t>(width);
    const uint32_t pixelHeight = static_cast<uint32_t>(height);

    const uint32_t blockWidth = pixelWidth >> 2;
    const uint32_t blockHeight = pixelHeight >> 2;
    const uint64_t byteSize = blockWidth * blockHeight * 16;
    const uint32_t rowStride = pixelWidth * componentCount;

    ThreadHeapSmartPointer<unsigned char> bcDest(ThreadHeapAllocator::Allocate(byteSize));

    unsigned char blockData[16][4];
    const unsigned char* pBlockData = &blockData[0][0];
    for (uint32_t blockY = 0; blockY < blockHeight; ++blockY)
    {
        for (uint32_t blockX = 0; blockX < blockWidth; ++blockX)
        {
            const uint32_t pixelBlockStartOffset = blockY * 4 * rowStride + blockX * 4;
            const uint32_t destBlockOffset = blockY * blockWidth + blockX;
            unsigned char* pDest = bcDest + destBlockOffset;

            for (uint32_t y = 0; y < 4; ++y)
            {
                for (uint32_t x = 0; x < 4; ++x)
                {
                    const uint32_t srcPixelOffset = pixelBlockStartOffset + y * rowStride + x;
                    const uint32_t blockDataDstOffset = y * 4 + x;
                    
                    blockData[blockDataDstOffset][0] = fileContent[srcPixelOffset];
                    blockData[blockDataDstOffset][1] = fileContent[srcPixelOffset + 1];
                    blockData[blockDataDstOffset][2] = fileContent[srcPixelOffset + 2];

                    if (componentCount == 4)
                    {
                        blockData[blockDataDstOffset][3] = fileContent[srcPixelOffset + 3];
                    }
                    else
                    {
                        blockData[blockDataDstOffset][3] = 255;
                    }
                }
            }

            constexpr int alpha = 1;
            stb_compress_dxt_block(pDest, pBlockData, alpha, STB_DXT_NORMAL);

            BIOME_ASSERT_ALWAYS_EXEC(fwrite(pBlockData, sizeof(uint8_t), sizeof(blockData), pDestFile) == sizeof(blockData));
        }
    }
    
    return TextureInfo { byteSize, pixelWidth, pixelHeight };
}
