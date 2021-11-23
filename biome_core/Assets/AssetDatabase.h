#pragma once

#include "biome_core/Assets/Texture.h"
#include "biome_core/Assets/Mesh.h"

namespace biome
{
    namespace asset
    {
        static constexpr size_t     cMaxRscFilePathLen = 1024;
        static constexpr uint32_t   cMagicNumber = 0xBADC0DE;
        static constexpr uint32_t   cVersion = 1;

        // Align header so data following it is properly aligned for either textures or meshes
        struct AssetDatabaseHeader
        {
            uint32_t    m_magicNumber { cMagicNumber };
            uint32_t    m_version { cVersion };
            uint32_t    m_meshCount { 0 };
            uint32_t    m_textureCount { 0 };
            char        m_pPackedBuffersFileName[16] { 0 };
            char        m_pPackedTexturesFileName[16] { 0 };
        };

        struct AssetDatabase
        {
            AssetDatabaseHeader m_header;
            uint8_t             m_data[1];
        };

        AssetDatabase*  LoadDatabase(const char* pFilePath);
        void            DestroyDatabase(AssetDatabase* pDatabase);
        const Texture*  GetTextures(const AssetDatabase* pDatabase);
        const Mesh*     GetMeshes(const AssetDatabase* pDatabase);
    }
}