#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float4 pos : POSITION;
	float4 col : COLOR;
	float3 norm: NORM;
	float2 uv : UV;
	float4 tangents :TAN;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float2 uvOut : UV;
	float4 colorOut : COLOR;
};

cbuffer OBJECT : register(b0)
{
	float4x4 WorldMatrix;
};

// TODO: PART 3 STEP 2a
cbuffer SCENE : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};


OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	float4 output = fromVertexBuffer.pos;

	output = mul(output, WorldMatrix);

	output = mul(output, viewMatrix);

	output = mul(output, projectionMatrix);

	sendToRasterizer.projectedCoordinate = output;

	sendToRasterizer.colorOut = fromVertexBuffer.col;

	sendToRasterizer.uvOut = fromVertexBuffer.uv;

	return sendToRasterizer;
}