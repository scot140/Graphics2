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
	float3 CamPos;
	float4 pos;
	float4 color;
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

	////First SpotLight Formula

	float4 textureColor = baseTexture.Sample(filter, input.uv);

	float4 lightDir = normalize(light.pos - input.WorldPos);

	float surfaceRatio = saturate(dot(-lightDir, float4(light.coneDir, 1)));

	float spotFactor = (surfaceRatio > light.coneWidth) ? 1 : 0;

	float lightRatio = 0;

	if (spotFactor == 1)
	{
		lightRatio = saturate(dot(lightDir, float4(input.norm, 0)));
	}

	float4 FirstLight = spotFactor * lightRatio * textureColor;

		if (spotFactor == 1)
		{

			float3 viewDir = normalize(light.CamPos - input.WorldPos.xyz);
				//float3 LDirection = float3(-lightDir.x, lightDir.y, lightDir.z);
				float3 HalfVector = normalize((lightDir.xyz) + viewDir);
				float SpecularFactor = saturate(dot(normalize(input.norm), normalize(HalfVector)));

			float4 specular = (float4)0;
				if (SpecularFactor > 0)
				{
					SpecularFactor = pow(SpecularFactor, 32);
					specular = float4(1, 1, 1, 1) * 1.0f * SpecularFactor;
				}

			FirstLight = FirstLight + specular;
		}

	////Second SpotLight Formula
	float4 lightDir2 = normalize(second.pos - input.WorldPos);

		float surfaceRatio2 = saturate(dot(-lightDir2, float4(second.coneDir, 1)));

	float spotFactor2 = (surfaceRatio > second.coneWidth) ? 1 : 0;

	float lightRatio2 = 0;

	if (spotFactor2 == 1)
	{
		lightRatio2 = saturate(dot(lightDir2, float4(input.norm, 0)));
	}

	float4 SecondLight = spotFactor2 * lightRatio2 * textureColor;

		///specular addition
		if (spotFactor2 == 1)
		{

			float3 viewDir = normalize(second.CamPos - input.WorldPos.xyz);
				//float3 LDirection = float3(-lightDir.x, lightDir.y, lightDir.z);
				float3 HalfVector = normalize((lightDir.xyz) + viewDir);
				float SpecularFactor = saturate(dot(normalize(input.norm), normalize(HalfVector)));

			float4 specular = (float4)0;
				if (SpecularFactor > 0)
				{
					SpecularFactor = pow(SpecularFactor, 32);
					specular = float4(1, 1, 1, 1) * 1.0f * SpecularFactor;
				}

			SecondLight = SecondLight + specular;
		}

		//Finalizing the color
	float4 finalColor = saturate(FirstLight + SecondLight);
	return float4(finalColor.xyz, textureColor.a);
}