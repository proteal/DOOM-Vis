#include "shared.hlsl"

struct VSIn
{
	float4 pos : POSITION;
};

struct VSOut
{
	float4 pos : SV_POSITION;
};

VSOut VS(VSIn input)
{
	VSOut output;

	output.pos = mul(input.pos, camWorldViewProj);

	return output;
}

float4 PS(VSOut input) : SV_Target
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
