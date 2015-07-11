
struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float3 uvOut : UV;
	float4 colorOut : COLOR;
};

TextureCube Skybox: register(t0);

SamplerState filter: register(s0);

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{
	float4 temp = Skybox.Sample(filter, input.uvOut);

	return temp;
}