#include "root_signature.hlsl"

struct Constants
{
    float4x4 view;
    float4x4 projection;
};

ConstantBuffer<Constants> cb;

struct VsOut
{
    float4 pos : SV_POSITION;
    float4 worldPos : TEXCOORD0;
};

[RootSignature(RootSig)]
VsOut main( float4 pos : POSITION )
{
    float4 worldPos = float4(pos.xyz, 1.f);
    float4x4 vp = mul(cb.view, cb.projection);
    float4 clipPos = mul(worldPos, vp);
    
    VsOut vsOut;
    vsOut.pos = clipPos;
    vsOut.worldPos = worldPos;

    return vsOut;
}