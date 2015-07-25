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
	float padding;
	float3 dir;
	float4 ambient;
	float4 diffuse;
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

	float4 textureColor = baseTexture.Sample(filter, input.uvOut);

	
	/*float4 finalColor;

	float ratio = dot(-light.dir, newNormal);

	finalColor = saturate(ratio * light.diffuse * textureColor);

	finalColor = light.ambient * finalColor;

	return float4(finalColor.xyz, textureColor.a);*/


	float ratio = dot(-light.dir, input.norm);
	float3 FirstLight = saturate(ratio * light.diffuse.rgb * textureColor.rgb);
		float3 FirstColor = light.ambient.rgb * FirstLight;

	float ratio2 = dot(-second.dir, input.norm);
	float3 SecondLight = saturate(ratio2 * second.diffuse.rgb * textureColor.rgb);
		float3 SecondColor = second.ambient.rgb * SecondLight;


		float3 Finalcolor = saturate(SecondColor + FirstColor);

	return float4(Finalcolor, textureColor.a);
}