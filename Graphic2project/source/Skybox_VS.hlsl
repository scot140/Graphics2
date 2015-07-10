#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float4 pos : POSITION;
	float4 col : COLOR;
	float3 norm: NORM;
	float2 uv : UV;

};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float3 uvOut : UV;
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

		sendToRasterizer.uvOut.x = fromVertexBuffer.pos.x;
	sendToRasterizer.uvOut.y = fromVertexBuffer.pos.y;
	sendToRasterizer.uvOut.z = fromVertexBuffer.pos.z;


	output = mul(output, WorldMatrix);

	output = mul(output, viewMatrix);

	output = mul(output, projectionMatrix);

	

	sendToRasterizer.projectedCoordinate = output;

	sendToRasterizer.colorOut = fromVertexBuffer.col;
	return sendToRasterizer;
}