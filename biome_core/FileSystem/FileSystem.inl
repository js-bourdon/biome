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
StaticArray<uint8_t, false, AllocatorType> biome::filesystem::ReadFileContent(const char* pSrcPath)
{
    FILE* pFile;
    errno_t err = fopen_s(&pFile, pSrcPath, "rb");

    if(err == 0 && pFile)
    {
        fseek(pFile, 0, SEEK_END);
        size_t fileSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        StaticArray<uint8_t, false, AllocatorType> content(fileSize);
        uint8_t* pContent = content.Data();

        const size_t bytesRead = fread(pContent, sizeof(uint8_t), fileSize, pFile);
        BIOME_ASSERT_MSG_FMT(bytesRead == fileSize, "File size: %zu Bytes read: %zu", fileSize, bytesRead);

        fclose(pFile);

        return content;
    }

    return StaticArray<uint8_t, false, AllocatorType>(0);
}
