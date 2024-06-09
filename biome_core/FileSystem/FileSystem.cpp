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

    return dirPath;
}

wstr_smart_ptr biome::filesystem::ExtractDirectoryPath(const wchar_t* pFilePath)
{
    const wchar_t* pDirectoryEnd = std::wcsrchr(pFilePath, '/');
    if (!pDirectoryEnd)
    {
        pDirectoryEnd = std::wcsrchr(pFilePath, '\\');
    }

	const size_t dirByteSize = static_cast<size_t>(reinterpret_cast<uintptr_t>(pDirectoryEnd) - reinterpret_cast<uintptr_t>(pFilePath));
    const size_t dirCharLen = dirByteSize / sizeof(wchar_t);
	wstr_smart_ptr dirPath = reinterpret_cast<char*>(ThreadHeapAllocator::Allocate(dirByteSize + sizeof(wchar_t)));
	dirPath[dirCharLen] = L'\0';
	memcpy(dirPath, pFilePath, dirByteSize);

	return dirPath;
}

str_smart_ptr biome::filesystem::AppendPaths(const char* pDirectoryPath, const char* pFilePath)
{
    const size_t dirPathLen = strlen(pDirectoryPath);
    const size_t filePathLen = strlen(pFilePath);
    const size_t destFilePathLen = dirPathLen + filePathLen + 1; // +1 for '\'

    str_smart_ptr destFilePath = ThreadHeapAllocator::Allocate(destFilePathLen + 1);

    destFilePath[destFilePathLen] = 0;
    destFilePath[dirPathLen] = '\\';
    memcpy(destFilePath, pDirectoryPath, dirPathLen);
    memcpy(destFilePath + dirPathLen + 1, pFilePath, filePathLen);

    return destFilePath;
}

wstr_smart_ptr biome::filesystem::AppendPaths(const wchar_t* pDirectoryPath, const wchar_t* pFilePath)
{
	const size_t dirPathLen = std::wcslen(pDirectoryPath);
	const size_t filePathLen = std::wcslen(pFilePath);
	const size_t destFilePathLen = dirPathLen + filePathLen + 1; // +1 for '\'

	wstr_smart_ptr destFilePath = ThreadHeapAllocator::Allocate((destFilePathLen + 1 ) * sizeof(wchar_t));

	destFilePath[destFilePathLen] = L'\0';
    destFilePath[dirPathLen] = L'\\';
	memcpy(destFilePath, pDirectoryPath, dirPathLen * sizeof(wchar_t));
	memcpy(destFilePath + dirPathLen + 1, pFilePath, filePathLen * sizeof(wchar_t));

	return destFilePath;
}

wstr_smart_ptr biome::filesystem::GetExecutableDirectory()
{
    constexpr DWORD bufferCharSize = 2048;
    wstr_smart_ptr destFilePath = ThreadHeapAllocator::Allocate(bufferCharSize * sizeof(wchar_t));
    GetModuleFileName(NULL, destFilePath, bufferCharSize);
    return ExtractDirectoryPath(destFilePath);
}
