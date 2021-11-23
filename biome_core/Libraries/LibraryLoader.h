#pragma once

#include "biome_rhi/Resources/ResourceHandles.h"

using namespace biome::rhi;

namespace biome
{
    namespace external
    {
        using LibraryHandle = biome::rhi::LibraryHandle;

        class LibraryLoader
        {
        public:

            static LibraryHandle    Load(const wchar_t* pName);
            static void             Unload(LibraryHandle handle);

        private:

            template<typename T>
            inline static void LoadFunction(LibraryHandle libraryHdl, const char *pFnctName, T &fnctPtr);

            // Platform-dependent implementations required.
            // Look into `Library/Implementations/<platform>.inl`
            static LibraryHandle LoadDynamicLibrary(const wchar_t *name);
            static void UnloadDynamicLibrary(LibraryHandle handle);
            static void* GetFunctionAddress(LibraryHandle lib, const char *functionName);

            LibraryLoader() = delete;
            ~LibraryLoader() = delete;
        };
    }
}