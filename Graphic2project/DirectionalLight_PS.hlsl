
struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float4 colorOut : COLOR;
	float3 norm: NORM;
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
	float3 normals = normalize(input.norm);

	float4 diffuse = baseTexture.Sample(filter, input.uvOut);

	float3 color;

	color = diffuse * light.ambient;

	color += saturate(dot(light.dir, input.norm)* light.diffuse * diffuse);

	return float4(color, diffuse.a);
}