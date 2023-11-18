struct VSOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

struct WorldConstants
{
    float4x4 World;
    float4x4 WorldView;
    float4x4 WorldViewProj;
};

struct Vertex
{
    float3 Position;
    float3 Normal;
};

struct VertexOut
{
    float4 PositionHS   : SV_Position;
};

struct AssetInfo
{
    uint indexBufferIndex;
    uint vertexBufferIndex;
    uint diffuseTextureIndex;
};

struct Meshlet
{
    uint indexBufferOffset;
    uint pouet;
};


ConstantBuffer<AssetInfo> AssetInfo : register(b0);
ConstantBuffer<WorldConstants> WorldConstants : register(b0, space1);

