
struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float2 uvOut : UV;
	float4 colorOut : COLOR;
};

texture2D Texture1 : register(t0);

texture2D Texture2 : register(t1);

SamplerState filter: register(s0);

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{
	float2 uvs = input.uvOut;

	float4 textureColor = Texture1.Sample(filter, uvs);
	float4 textureCol = Texture2.Sample(filter, uvs);

	float4 blend = textureColor * textureCol* 2.0f;

	blend = saturate(blend);

	return blend;
}