#include "root_signature.hlsl"
#include "common.hlsli"

[RootSignature(RootSig)]
float4 PSMain(VSOut vsIn) : SV_TARGET
{
	return float4(vsIn.uv, 0.0f, 1.0f);
}