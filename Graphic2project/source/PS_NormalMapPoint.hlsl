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
texture2D normalMap : register(t1);

SamplerState filter: register(s0);

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{
	float4 textureColor = baseTexture.Sample(filter, input.uv);
	float3 newNormal = normalMap.Sample(filter, input.uv).xyz;
	newNormal.g = 1 - newNormal.g;

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

	//Point light Algorithm

	////////////////////////////
	float4 lightDirection = normalize(light.pos - input.WorldPos);

		float ratio = saturate(dot(lightDirection.xyz, newNormal));

	float atten = 1.0 - saturate(length(light.pos - input.WorldPos) / light.range);

	atten = pow(atten, light.power);

	float4 finalColor = ratio * light.ambient * textureColor * atten;



		return float4(finalColor.xyz, textureColor.a);
}