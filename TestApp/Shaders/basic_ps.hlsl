struct VsOut
{
    float4 pos : SV_POSITION;
    float4 worldPos : TEXCOORD0;
};

float4 main(VsOut vsOut) : SV_TARGET
{
	float3 sun_direction = normalize(float3(1.f, 1.f, 1.f));
	float3 diffX = normalize(ddx(vsOut.worldPos.xyz));
	float3 diffY = normalize(ddy(vsOut.worldPos.xyz));
	float3 surface_normal = normalize(cross(diffX, diffY));
	float intensity = dot(sun_direction, surface_normal);
	return float4(intensity, intensity, intensity, 1.0f);
}