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
	float2 uv = (float2)0;
	uv.x = input.uvOut.x;

	if (width > 1)
	{
		uv.x /= maxFrame;
		float framelength = width / maxFrame;
		uv.x += (frame * framelength) / width;
		//uv.y += (frame * framelength) / width;
	}

	uv.y = input.uvOut.y;

	uv = float2(uv.x, input.uvOut.y);


	float4 temp = baseTexture.Sample(filter, uv);


		return temp;

}