#include "shared.hlsl"

struct VSIn
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float3 posWS : TEXCOORD0;
	float3 normal : TEXCOORD1;
};

VSOut VS(VSIn input)
{
	VSOut output;

	output.pos = mul(input.pos, worldViewProj);
	output.posWS = mul(input.pos, world).xyz;
	output.normal = normalize(mul(float4(input.normal, 0.0f), world).xyz);

	return output;
}

float4 PS(VSOut input) : SV_Target
{
	float3 toPlayer = playerPos - input.posWS;

	float normFactor = pow(abs(dot(normalize(input.normal), normalize(toPlayer))), 0.33);
	float distFactor = max(0.0f, 1.0f - length(toPlayer) / 1500.0f);

	float color = normFactor * distFactor * distFactor;
	color = lerp(color, 1.0f, 0.01f);

	return float4(color, color, color, 1.0f);
}
