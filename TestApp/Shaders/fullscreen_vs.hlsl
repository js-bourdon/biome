#include "root_signature.hlsl"
#include "common.hlsli"

[RootSignature(RootSig)]
VSOut VSMain(uint vertId : SV_VertexID)
{
	const float4 pos[] =
	{
		{ -1.f, -1.f, 0.f, 1.f },
		{ -1.f,  3.f, 0.f, 1.f },
		{  3.f, -1.f, 0.f, 1.f }
	};

	const float2 uv[] =
	{
		{ 0.f,  1.f },
		{ 0.f, -1.f },
		{ 2.f,  0.f }
	};

	uint vertIndex = vertId % 3;

	VSOut vsOut = { pos[vertIndex], uv[vertIndex] };

	return vsOut;
}