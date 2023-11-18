#include <pch.h>
#include <direct.h>
#include "FileSystem.h"

using namespace biome::filesystem;
using namespace biome::memory;

bool biome::filesystem::FileExists(const char* pFilePath)
{
    FILE* pFile;

    if (fopen_s(&pFile, pFilePath, "r") == 0)
    {
        fclose(pFile);
        return true;
    }

    return false;
}

bool biome::filesystem::DirectoryExists(const char* pDirectoryPath)
{
    struct stat dirStat;
    if (stat(pDirectoryPath, &dirStat) >= 0)
    {
        return true;
    }

    return false;
}

bool biome::filesystem::CreateDirectory(const char* pDirectoryPath)
{
    bool hasSuccessfullyCreateDirectory = true;

    uintptr_t dirPathStart = reinterpret_cast<uintptr_t>(pDirectoryPath);
    size_t pathLen = strlen(pDirectoryPath);
    str_smart_ptr tmpPath = ThreadHeapAllocator::Allocate(pathLen + 1);
    tmpPath[pathLen] = 0;

    const char* pSubPath = pDirectoryPath;

    do
    {
        pSubPath = strchr(pSubPath + 1, '/');
        if (pSubPath)
        {
            size_t subPathLen = static_cast<size_t>(reinterpret_cast<uintptr_t>(pSubPath) - dirPathStart) + 1;
            memcpy(tmpPath, pDirectoryPath, subPathLen);
            tmpPath[subPathLen] = 0;

            if (!DirectoryExists(tmpPath))
            {
                hasSuccessfullyCreateDirectory &= (_mkdir(tmpPath) == 0);
            }
        }
    } while (pSubPath);

    if (!DirectoryExists(pDirectoryPath))
    {
        hasSuccessfullyCreateDirectory &= (_mkdir(pDirectoryPath) == 0);
    }

    return hasSuccessfullyCreateDirectory;
}

str_smart_ptr biome::filesystem::ExtractDirectoryPath(const char* pFilePath)
{
    const char* pDirectoryEnd = strrchr(pFilePath, '/');
    size_t dirLen = static_cast<size_t>(reinterpret_cast<uintptr_t>(pDirectoryEnd) - reinterpret_cast<uintptr_t>(pFilePath));
    str_smart_ptr dirPath = reinterpret_cast<char*>(ThreadHeapAllocator::Allocate(dirLen + 1));
    dirPath[dirLen] = 0;
    memcpy(dirPath, pFilePath, dirLen);

    return std::move(dirPath);
}

str_smart_ptr biome::filesystem::AppendPaths(const char* pDirectoryPath, const char* pFilePath)
{
    size_t dirPathLen = strlen(pDirectoryPath);
    size_t filePathLen = strlen(pFilePath);

    size_t destFilePathLen = dirPathLen + filePathLen;
    str_smart_ptr destFilePath = ThreadHeapAllocator::Allocate(destFilePathLen);

    destFilePath[destFilePathLen] = 0;
    memcpy(destFilePath, pDirectoryPath, dirPathLen);
    memcpy(destFilePath + dirPathLen, pFilePath, filePathLen);

    return std::move(destFilePath);
}
