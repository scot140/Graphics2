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
	float3 dir;
	float4 ambient;
	float4 diffuse;
};

cbuffer cPerPixel : register(b0)
{
	Light light;
}

texture2D baseTexture : register(t0);

SamplerState filter: register(s0);

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{

	float4 diffuse = baseTexture.Sample(filter, input.uvOut);

	float3 color;

	float3 ratio = dot(-light.dir, input.norm);

		color = saturate(ratio * light.diffuse.rgb * diffuse.rgb);

	color = light.ambient.rgb * color;
	return float4(color, diffuse.a);
}