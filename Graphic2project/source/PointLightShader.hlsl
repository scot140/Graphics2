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
	float  range;
	float  power;
	float2 padding;
	float4 ambient;
	float4 pos;
};

cbuffer cPerPixel : register(b0)
{
	Light light;
	Light second;
}

texture2D baseTexture : register(t0);

SamplerState filter: register(s0);

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{
	//sampleing the texture
	float4 textureColor = baseTexture.Sample(filter, input.uvOut);

	//first light Point formula
	float4 lightDirection = normalize(light.pos - input.WorldPos);

	float ratio = saturate(dot(normalize(lightDirection.xyz), input.norm));

	float atten = 1.0 - saturate(length(light.pos - input.WorldPos) / light.range);

	atten = pow(atten, light.power);

	float4 FirstLight = ratio * light.ambient * textureColor * atten;

		//second light point formula
		float4 lightDirection2 = normalize(second.pos - input.WorldPos);

		float ratio2 = saturate(dot(normalize(lightDirection2.xyz), input.norm));

	float atten2 = 1.0 - saturate(length(second.pos - input.WorldPos) / second.range);

	atten2 = pow(atten2, second.power);

	float4 SecondLight = ratio2 * second.ambient * textureColor * atten2;

		//combinding
		float4 finalColor = saturate(FirstLight + SecondLight);

		return float4(finalColor.rgb, textureColor.a);
}