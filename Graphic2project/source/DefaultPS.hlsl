
struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float2 uvOut : UV;
	float4 colorOut : COLOR;
};

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{
	return input.colorOut;
}