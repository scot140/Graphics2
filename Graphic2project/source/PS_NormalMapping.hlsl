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
	float3 dir;
	float4 ambient;
	float4 diffuse;
};

cbuffer cPerPixel : register(b0)
{
	Light light;
}

texture2D baseTexture : register(t0);
texture2D normalMap : register(t1);

SamplerState filter: register(s0);

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{
	float4 textureColor = baseTexture.Sample(filter, input.uv);
	float3 newNormal = normalMap.Sample(filter, input.uv).xyz;
	//newNormal.g = 1 - newNormal.g;

	//resizing the range
	newNormal = (newNormal * 2.0f) - 1.0f;
	//normalizing the input values
	float3 norm = input.norm;
	float3 Tangent = normalize(input.Tangent);
	float3 Binormal = normalize(input.Binormal);

	float3x3 MatrixTBN;
	//creating a TBN matriz
	MatrixTBN[0] = Tangent;
	MatrixTBN[1] = Binormal;
	MatrixTBN[2] = norm;

	newNormal = mul(newNormal, MatrixTBN);
	

	float4 finalColor;

	float ratio = dot(-light.dir, newNormal);

	finalColor = saturate(ratio * light.diffuse * textureColor);

	finalColor = light.ambient * finalColor;

	return float4(finalColor.xyz, textureColor.a);
}