#include "root_signature.hlsl"

[RootSignature(RootSig)]
float4 main( float4 pos : POSITION ) : SV_POSITION
{
    float4 finalPos = float4(pos.xyz, 1.f);

    float4x4 v =
    {
        1.f,    0.f,    0.f,    0.f,
        0.f,    1.f,    0.f,    0.f,
        0.f,    0.f,    1.f,    0.f,
        0.f,    0.f,    -50.f,  1.f,
    };

    float4x4 p = 
    {
        54.7072f,   0.f,        0.f,        0.f,
        0.f,        97.2647f,   0.f,        0.f,
        0.f,        0.f,        1.0101f,    1.f,
        0.f,        0.f,        -5.0505f,   0.f
    };

    float4x4 vp = mul(v, p);

    return mul(pos, vp);
}