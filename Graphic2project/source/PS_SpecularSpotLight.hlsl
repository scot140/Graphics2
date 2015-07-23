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
	float power;
	float3 coneDir;
	float coneWidth; // use Radians please do not forget
	float3 CamPos;
	float4 pos;
	float4 color;
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

	//Spotlight Algorithm
	float4 lightDir = normalize(light.pos - input.WorldPos);

		float surfaceRatio = saturate(dot(-normalize(lightDir), float4(normalize(light.coneDir).xyz, 1)));

	float spotFactor = (surfaceRatio > light.coneWidth) ? 1 : 0;

	float lightRatio = 0;

	if (spotFactor == 1)
	{
		lightRatio = saturate(dot(lightDir, float4(newNormal, 0)));
	}

	float4 finalColor = spotFactor * lightRatio * textureColor;

		if (spotFactor == 1)
		{

			float3 viewDir = normalize(light.CamPos - input.WorldPos.xyz);
				float3 LDirection = float3(-lightDir.x, lightDir.y, lightDir.z);
				float3 HalfVector = normalize((-lightDir.xyz) + viewDir);
				float SpecularFactor = saturate(dot(newNormal, normalize(HalfVector)));

			float4 specular = (float4)0;
				if (SpecularFactor > 0)
				{
					SpecularFactor = pow(SpecularFactor, 64);
					specular = float4(1, 1, 1, 1) * 1.0f * SpecularFactor;
				}

			finalColor = finalColor + specular;
		}

	return float4(finalColor.rgb, textureColor.a);
}