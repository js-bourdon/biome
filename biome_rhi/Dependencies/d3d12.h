#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 600; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\"; }
