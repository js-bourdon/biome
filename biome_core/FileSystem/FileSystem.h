#pragma once

#include "biome_core/Memory/ThreadHeapSmartPointer.h"
#include "biome_core/DataStructures/StaticArray.h"

using namespace biome::memory;
using namespace biome::data;

namespace biome
{
    namespace filesystem
    {
        struct FileHandleRAII
        {
            FileHandleRAII(FILE* pFile) : m_pFile(pFile) {}
            ~FileHandleRAII() { fclose(m_pFile); }
            FILE* m_pFile;
        };

        bool FileExists(const char* pFilePath);
        bool DirectoryExists(const char* pDirectoryPath);
        bool CreateDirectory(const char* pDirectoryPath);

        template<typename AllocatorType = ThreadHeapAllocator>
        uint8_t* ReadFileContent(const char* pSrcPath, size_t& o_FileSize);

        template<typename AllocatorType = ThreadHeapAllocator>
        StaticArray<uint8_t, false, AllocatorType> ReadFileContent(const char* pSrcPath);

        str_smart_ptr&& ExtractDirectoryPath(const char* pFilePath);
        str_smart_ptr&& AppendPaths(const char* pDirectoryPath, const char* pFilePath);
    }
}

#include "biome_core/FileSystem/FileSystem.inl"
