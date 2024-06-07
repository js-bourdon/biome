#include "basic_common.hlsli"
#include "root_signature.hlsl"

[RootSignature(RootSig)]
VsOut main(VsIn vsIn)
{
    float4 worldPos = float4(vsIn.pos.xyz, 1.f);
    float4x4 vp = mul(cb.view, cb.projection);
    float4 clipPos = mul(worldPos, vp);
    
    VsOut vsOut;
    vsOut.pos = clipPos;
    vsOut.worldPos = worldPos;
    vsOut.normal = vsIn.normal;
    vsOut.uv = vsIn.uv;

    return vsOut;
}