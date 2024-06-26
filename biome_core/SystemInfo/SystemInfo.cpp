#include <pch.h>
#include "SystemInfo.h"

using namespace biome::system;

static CPUArchitecture Convert(WORD arch)
{
    switch (arch)
    {
    case 9: // PROCESS_ARCHITECTURE_AMD64
        return CPUArchitecture::x64;
    case 12: // PROCESSOR_ARCHITECTURE_ARM64
        return CPUArchitecture::ARM64;
    }

    return CPUArchitecture::Unsupported;
}

SystemInfo biome::system::GetSystemInfo()
{
    SystemInfo info;

    SYSTEM_INFO nativeInfo;
    ::GetSystemInfo(&nativeInfo);

    info.m_AllocationGranularity = nativeInfo.dwAllocationGranularity;
    info.m_AllocationPageSize = nativeInfo.dwPageSize;
    info.m_CpuArchitecture = Convert(nativeInfo.wProcessorArchitecture);
    info.m_LogicalCpuCoreCount = nativeInfo.dwNumberOfProcessors;

    return info;
}
