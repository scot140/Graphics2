
struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float4 colorOut : COLOR;
	float3 normals: NORM;
	float2 uvOut : UV;
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

	float3 ratio = dot(-light.dir, input.normals);

		color = saturate(ratio * light.diffuse * diffuse);

	return float4(color, diffuse.a);
}