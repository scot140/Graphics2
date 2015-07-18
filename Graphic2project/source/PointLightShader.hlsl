struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float4 WorldPos : POSITION;
	float3 norm: NORM;
	float2 uvOut : UV;
	float3 padding : padding;
};

struct Light
{
	float4 pos;
	float  range;
	float  power;
	float4 ambient;
	float2 padding;
};

cbuffer cPerPixel : register(b0)
{
	Light light;
}

texture2D baseTexture : register(t0);

SamplerState filter: register(s0);

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{
	float4 color = baseTexture.Sample(filter, input.uvOut);

	float4 lightDirection = normalize(light.pos - input.WorldPos);

	float ratio = saturate(dot(lightDirection.xyz, input.norm));

	float atten = 1.0 - saturate(length(light.pos - input.WorldPos) / light.range);

	atten = pow(atten, light.power);

	float4 finalColor = ratio * light.ambient * color * atten;

	return float4(finalColor.xyz,color.a);
}