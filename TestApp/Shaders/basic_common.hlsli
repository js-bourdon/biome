struct Constants
{
    float4x4 view;
    float4x4 projection;
};

struct TextureOffsets
{
    uint albedo;
    uint roughness;
};

struct VsIn
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VsOut
{
    float4 pos : SV_POSITION;
    float4 worldPos : TEXCOORD0;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD1;
};

ConstantBuffer<Constants> cb : register(b0);
ConstantBuffer<TextureOffsets> textureOffsets : register(b1);

SamplerState smplr : register(s0);
