struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float4 WorldPos : POSITION;
	float3 norm: NORM;
	float2 uv : UV;
	float3 padding : padding;
};

struct Light
{
	float power;
	float3 coneDir;
	float coneWidth; // use Radians please do not forget
	float3 padding;
	float4 pos;
	float4 color;
};

cbuffer cPerPixel : register(b0)
{
	Light light;
}

texture2D baseTexture : register(t0);

SamplerState filter: register(s0);

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{

	float4 textureColor = baseTexture.Sample(filter, input.uv);

	float4 lightDir = normalize(light.pos - input.WorldPos);

	float surfaceRatio = saturate(dot(-lightDir, float4(light.coneDir,1)));

	float spotFactor = (surfaceRatio > light.coneWidth) ? 1 : 0;

	float lightRatio = 0;

	if (spotFactor == 1)
	{
		lightRatio = saturate(dot(lightDir, float4(input.norm, 0)));
	}

	float4 finalColor = spotFactor * lightRatio * textureColor;

		return float4(finalColor.xyz, textureColor.a);
}