#include <pch.h>
#include <type_traits>
#include <stdio.h>
#include "LibraryLoader.h"

using namespace biome::rhi;
using namespace biome::external;

static wchar_t g_LibraryFileName[2048];

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

LibraryHandle LibraryLoader::LoadDynamicLibrary(const wchar_t* name)
{
    const size_t strLength = wcslen(name);
    const size_t bufferSize = (strLength + 5) * sizeof(wchar_t);

    swprintf_s(g_LibraryFileName, BIOME_ARRAY_SIZE(g_LibraryFileName), L"%s.dll", name);
    LibraryHandle libHdl = reinterpret_cast<LibraryHandle>(LoadLibraryW(g_LibraryFileName));

    return libHdl;
}

void LibraryLoader::UnloadDynamicLibrary(LibraryHandle handle)
{
    FreeLibrary(reinterpret_cast<HMODULE>(handle));
}

void* LibraryLoader::GetFunctionAddress(LibraryHandle lib, const char* functionName)
{
    return GetProcAddress(reinterpret_cast<HMODULE>(lib), functionName);
}
