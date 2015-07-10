struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float2 uvOut: UV;
	float4 colorOut : COLOR;
};

texture2D baseTexture: register(t0);

SamplerState filter: register(s0);

cbuffer TIMER : register(b0)
{
	float frame;
	float maxFrame;
	float width;
	float padding;
};

float4 main(OUTPUT_VERTEX input) : SV_TARGET
{
	//misc math for finding out the width of the each frame
	float2 uv = input.uvOut;
	if (width > 1)
	{
		uv.x /= maxFrame;
		float framelength = width / maxFrame;
		uv.x += (frame * framelength) / width;
		//uv.y += (frame * framelength) / width;
	}

	float4 temp = baseTexture.Sample(filter, uv);

		float4 baseColor;

	baseColor.a = temp.b;
	baseColor.r = temp.g;
	baseColor.g = temp.r;
	baseColor.b = temp.a;

	return temp;

}