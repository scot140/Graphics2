#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float4 pos : POSITION;
	float4 col : COLOR;
	float3 norm: NORM;
	float2 uv : UV;
	float4 tangents :TAN;
	float3 instPos : IPOSITION;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float4 WorldPos : POSITION;
	float3 norm: NORM;
	float2 uvOut : UV;
	float3 padding : padding;
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

		output += float4(fromVertexBuffer.instPos, 0);

	output = mul(output, WorldMatrix);

	sendToRasterizer.WorldPos = output;

	output = mul(output, viewMatrix);

	output = mul(output, projectionMatrix);

	sendToRasterizer.projectedCoordinate = output;

	sendToRasterizer.uvOut = fromVertexBuffer.uv;

	float4 normal = float4(fromVertexBuffer.norm.xyz, 0);

		sendToRasterizer.norm = mul(normal, WorldMatrix).xyz;

	return sendToRasterizer;
}