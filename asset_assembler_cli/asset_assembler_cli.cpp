#include "asset_assembler/database/AssetDatabaseBuilder.h"
#include "biome_core/Memory/ThreadHeapAllocator.h"
#include "biome_core/Core/Defines.h"
#include "biome_core/FileSystem/FileSystem.h"

using namespace asset_assembler::database;
using namespace biome::memory;
using namespace biome;

int main(int argc, char* argv[])
{
    BIOME_ASSERT_ALWAYS_EXEC(ThreadHeapAllocator::Initialize(GiB(1), MiB(100)));

    // All heavy memory allocations must go through biome::memory::VirtualMemoryAllocator.
    AssetDatabaseBuilder builder;

//     BIOME_ASSERT(argc >= 3);
//     const char* pGltfFilePath = argv[1];
//     const char* pDbFilePath = argv[2];

    bool success = builder.BuildDatabase(
        "../TestApp/Media/star_trek_danube_class/scene.gltf", 
        "../TestApp/Media/builds/star_trek_danube_class/StartTrek.db");
    //bool success = builder.BuildDatabase(pGltfFilePath, pDbFilePath);

    if (success)
    {
        printf_s("Asset generation successful");
    }
    else
    {
        printf_s("Asset generation FAILED!");
    }

    return 0;
}

