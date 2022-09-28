#include <pch.h>
#include <type_traits>
#include "LibraryLoader.h"

using namespace biome::rhi;
using namespace biome::external;

///////////////////////////////////////////////////////////////////////////
// LibraryLoader
///////////////////////////////////////////////////////////////////////////
LibraryHandle LibraryLoader::Load(const wchar_t* pName)
{
    return LoadDynamicLibrary(pName);
}

void LibraryLoader::Unload(LibraryHandle handle)
{
    UnloadDynamicLibrary(handle);
}

#include INCLUDE_IMPLEMENTATION(LibraryLoader)
