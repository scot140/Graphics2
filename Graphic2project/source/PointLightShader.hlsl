struct OUTPUT_VERTEX
{
	float4 pos : SV_POSITION;
	float4 colorOut : COLOR;
	float4 WorldPos : POSITION;
	float3 norm: NORM;
	float2 uvOut : UV;
	float3 padding : padding;
};

struct Light
{
	float4 pos;
	float  range;
	float  power;
	float4 ambient;
	float4 diffuse;
	float2 padding;
};

cbuffer cPerPixel : register(b0)
{
	Light light;
}

texture2D baseTexture : register(t0);

SamplerState filter: register(s0);

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{
	float4 color = float4(0, 0, 0, 1);
	//
	float4 diffuse = baseTexture.Sample(filter, input.uvOut);

	float direction = length(light.pos - input.WorldPos);

	float4 ambient = diffuse * light.ambient;

		// if the distance bewteen the pixel is with in  the lights touch
		if (direction > light.range)
		{
			return float4(ambient.rgb, diffuse.a);
		}

	//color = diffuse;

	// normalizing the direction
	float4 normDistance = (light.pos - input.WorldPos) / direction;

		///Calculating the attenuation
		float4 atten = 1.0 - saturate(direction / light.range);
		atten = pow(atten, light.power);


	/// finding the percentage of the this pixel and the light
	float ratio = dot(normDistance.xyz, input.norm);


	///adding the attenuation if you are are with  in the light
	if (ratio > 0.0f)
	{
		color += ratio * diffuse * light.diffuse;

		color /= atten.x + (atten.y * direction) + (atten.z *(direction*direction));
	}

	float4 Finalcolor = saturate(color + ambient);
		return float4(Finalcolor.rgb, diffuse.a);
}