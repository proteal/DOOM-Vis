
SamplerState pointSampler;
SamplerState filterSampler;

Texture2D<float3> emuTexture;

cbuffer WallData
{
	float4x4 worldViewProj;
	float4x4 world;
};

cbuffer PlayerData
{
	float3 playerPos;
};

cbuffer CamData
{
	float4x4 camWorldViewProj;
};

cbuffer PlaneData
{
	float4x4 planeWorldViewProj;
	float3 planeColor;
};
