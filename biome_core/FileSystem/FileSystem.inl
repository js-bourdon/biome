#pragma once

template<typename AllocatorType>
uint8_t* biome::filesystem::ReadFileContent(const char* pSrcPath, size_t& o_FileSize)
{
    uint8_t* pContent = nullptr;
    FILE* pFile;
    errno_t err = fopen_s(&pFile, pSrcPath, "r");
    o_FileSize = 0;

    if (err == 0 && pFile)
    {
        fseek(pFile, 0, SEEK_END);
        size_t fileSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        pContent = static_cast<uint8_t*>(AllocatorType::Allocate(fileSize));

        fread(pContent, sizeof(uint8_t), fileSize, pFile);
        fclose(pFile);

        o_FileSize = fileSize;
    }

    return pContent;
}

template<typename AllocatorType>
StaticArray<uint8_t, AllocatorType> biome::filesystem::ReadFileContent(const char* pSrcPath)
{
    FILE* pFile;
    errno_t err = fopen_s(&pFile, pSrcPath, "r");

    if(err == 0 && pFile)
    {
        fseek(pFile, 0, SEEK_END);
        size_t fileSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        StaticArray<uint8_t, AllocatorType> content(fileSize);
        uint8_t* pContent = content.Data();

        fread(pContent, sizeof(uint8_t), fileSize, pFile);
        fclose(pFile);

        return content;
    }

    return StaticArray<uint8_t, AllocatorType>(0);
}
