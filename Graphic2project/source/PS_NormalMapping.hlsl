struct OUTPUT_VERTEX
{
	float4 pos : SV_POSITION;
	float3 Tangent : TAN;
	float4 WorldPos : POSITION;
	float3 norm: NORM;
	float2 uv : UV;
	float3 Binormal : BiNORM;
	float padding : padding;
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
texture2D normalMap : register(t1);

SamplerState filter: register(s0);

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{
	float4 textureColor = baseTexture.Sample(filter, input.uv);
	float3 newNormal = normalMap.Sample(filter, input.uv).xyz;
	

	//resizing the range
	newNormal = (newNormal * 2.0f) - 1.0f;
	//normalizing the input values
	float3 norm = normalize(input.norm);
	float3 Tangent = normalize(input.Tangent);
	float3 Binormal = normalize(input.Binormal);

	float3x3 MatrixTBN;
	//creating a TBN matriz
	MatrixTBN[0] = Tangent;
	MatrixTBN[1] = Binormal;
	MatrixTBN[2] = norm;

	newNormal = mul(newNormal, MatrixTBN);
	
	/*float4 finalColor;

	float ratio = dot(-light.dir, newNormal);

	finalColor = saturate(ratio * light.diffuse * textureColor);

	finalColor = light.ambient * finalColor;

	return float4(finalColor.xyz, textureColor.a);*/


	float ratio = dot(-light.dir, newNormal);
	float3 FirstLight = saturate(ratio * light.diffuse.rgb * textureColor.rgb);
		float3 FirstColor = light.ambient.rgb * FirstLight;

		float ratio2 = dot(-second.dir, newNormal);
	float3 SecondLight = saturate(ratio2 * second.diffuse.rgb * textureColor.rgb);
		float3 SecondColor = second.ambient.rgb * SecondLight;


		float3 Finalcolor = saturate(SecondColor + FirstColor);

	return float4(Finalcolor, textureColor.a);
}