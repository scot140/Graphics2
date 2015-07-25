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

	//Spotlight Algorithm
	float4 lightDir = normalize(light.pos - input.WorldPos);

		float surfaceRatio = saturate(dot(-normalize(lightDir), float4(normalize(light.coneDir).xyz, 1)));

	float spotFactor = (surfaceRatio > light.coneWidth) ? 1 : 0;

	float lightRatio = 0;

	if (spotFactor == 1)
	{
		lightRatio = saturate(dot(lightDir, float4(newNormal, 0)));
	}

	float4 FirstLight = spotFactor * lightRatio * textureColor;

		///if the spot is lighting something check to see if I should apply a specular shine to it
		if (spotFactor == 1)
		{
			//finding the camera distance form the point
			float3 viewDir = normalize(light.CamPos - input.WorldPos.xyz);
				//the Direction of the light
				float3 LDirection = float3(-lightDir.x, -lightDir.y, -lightDir.z);
				float3 HalfVector = normalize((lightDir.xyz) + viewDir);
				float SpecularFactor = saturate(dot(newNormal, normalize(HalfVector)));

			//if the specular Factor is in the range of the shine give it that white shine

			float4 specular = (float4)0;
				if (SpecularFactor > 0)
				{
					SpecularFactor = pow(SpecularFactor, 64);
					specular = float4(1, 1, 1, 1) * 1.0f * SpecularFactor;
				}

			FirstLight = FirstLight + specular;
		}

	//Second  Spotlight Algorithm
	float4 lightDir2 = normalize(second.pos - input.WorldPos);

		float surfaceRatio2 = saturate(dot(-normalize(lightDir2), float4(normalize(second.coneDir).xyz, 1)));

	float spotFactor2 = (surfaceRatio2 > second.coneWidth) ? 1 : 0;

	float lightRatio2 = 0;

	//seconding to see if the light in in the spot second... it if is light or not
	if (spotFactor2 == 1)
	{
		lightRatio2 = saturate(dot(lightDir2, float4(newNormal, 0)));
	}

	//applying the light to the texture
	float4 SecondLight = spotFactor2 * lightRatio2 * textureColor;

		///Finalizing the color

		///if the spot is lighting something check to see if I should apply a specular shine to it
		if (spotFactor2 == 1)
		{
			//finding the camera distance form the point
			float3 viewDir = normalize(second.CamPos - input.WorldPos.xyz);
				//the Direction of the light
				float3 LDirection = float3(-lightDir2.x, -lightDir2.y, -lightDir2.z);
				float3 HalfVector = normalize((lightDir2.xyz) + viewDir);
				float SpecularFactor = saturate(dot(newNormal, normalize(HalfVector)));

			//if the specular Factor is in the range of the shine give it that white shine

			float4 specular = (float4)0;
				if (SpecularFactor > 0)
				{
					SpecularFactor = pow(SpecularFactor, 64);
					specular = float4(1, 1, 1, 1) * 1.0f * SpecularFactor;
				}

			SecondLight = SecondLight + specular;
		}

	float4 finalColor = saturate(FirstLight + SecondLight);

		return float4(finalColor.rgb, textureColor.a);
}