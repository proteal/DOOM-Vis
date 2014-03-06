#include "shared.hlsl"

struct VSIn
{
	float4 pos : POSITION;
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

VSOut VS(VSIn input)
{
	VSOut output;

    output.pos = input.pos;
	output.uv = float2(input.pos.xy * float2(0.5f, -0.5f) + 0.5f);

	return output;
}

float4 PS(VSOut input) : SV_Target
{
	float3 color = emuTexture.Sample(filterSampler, input.uv);
	return float4(color, 1.0f);
}
