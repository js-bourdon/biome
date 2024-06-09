#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#ifdef _DEBUG
	#include <dxgidebug.h>
#endif

using namespace Microsoft::WRL;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#ifdef _DEBUG
	#pragma comment(lib, "dxguid.lib")
#endif

extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 614; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\"; }
