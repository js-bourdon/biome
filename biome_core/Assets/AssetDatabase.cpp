#include <pch.h>
#include "AssetDatabase.h"
#include "biome_core/FileSystem/FileSystem.h"
#include "biome_core/Assets/Texture.h"

using namespace biome::asset;
using namespace biome::filesystem;

AssetDatabase* biome::asset::LoadDatabase(const char* pFilePath)
{
    size_t fileSize;
    uint8_t* pData = ReadFileContent<ThreadHeapAllocator>(pFilePath, fileSize);

    if (fileSize < sizeof(AssetDatabaseHeader))
    {
        BIOME_ASSERT_MSG(false, "Invalid database file");
        return nullptr;
    }

    AssetDatabase* pDatabase = reinterpret_cast<AssetDatabase*>(pData);
    if (pDatabase->m_header.m_magicNumber != cMagicNumber)
    {
        BIOME_ASSERT_MSG(false, "Invalid database. Wrong magic number.");
        return nullptr;
    }

    return pDatabase;
}

void biome::asset::DestroyDatabase(AssetDatabase* pDatabase)
{
    ThreadHeapAllocator::Release(pDatabase);
}

const Texture* biome::asset::GetTextures(const AssetDatabase* pDatabase)
{
    const Texture* pTextures = reinterpret_cast<const Texture*>(pDatabase->m_data);
    return pTextures;
}

const Mesh* biome::asset::GetMeshes(const AssetDatabase* pDatabase)
{
    const uint32_t textureCount = pDatabase->m_header.m_textureCount;
    const Mesh* pMeshes = reinterpret_cast<const Mesh*>(pDatabase->m_data + (textureCount * sizeof(Texture)));
    return pMeshes;
}