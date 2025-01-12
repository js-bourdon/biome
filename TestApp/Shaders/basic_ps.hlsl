#include "basic_common.hlsli"
#include "root_signature.hlsl"

[RootSignature(RootSig)]
float4 main(VsOut vsOut) : SV_TARGET
{
    const float3 sun_direction = normalize(float3(1.f, 1.f, 1.f));
    float3 surface_normal = normalize(vsOut.normal);
    float intensity = dot(sun_direction, surface_normal);
    
    Texture2D<float4> albedoTex = ResourceDescriptorHeap[textureOffsets.albedo];
    float4 albedo = albedoTex.Sample(smplr, vsOut.uv);
    
    //float3 color = float3(intensity * vsOut.uv, intensity);
    float3 color = albedo.xyz * intensity;
    
    return float4(color, 1.0f);
}