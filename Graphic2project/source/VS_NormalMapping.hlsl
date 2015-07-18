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
	float4 pos : SV_POSITION;
	float3 Tangent : TAN;
	float4 WorldPos : POSITION;
	float3 norm: NORM;
	float2 uv : UV;
	float3 Binormal : BiNORM;
	float padding : padding;
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

	sendToRasterizer.WorldPos = output;

	output = mul(output, viewMatrix);

	output = mul(output, projectionMatrix);


	sendToRasterizer.pos = output;

	sendToRasterizer.uv = fromVertexBuffer.uv;

	///getting the three vectors fot he TBN matrix
	float4 normal = float4(fromVertexBuffer.norm.xyz,0);

		sendToRasterizer.norm = mul(normal, WorldMatrix).xyz;

	float4 tan = fromVertexBuffer.tangents;

		sendToRasterizer.Tangent = mul(float4(tan.xyz* tan.w, 0.0f), WorldMatrix).xyz;

	float4 crossProd = float4(cross(sendToRasterizer.norm, sendToRasterizer.Tangent), 0.0f);

		sendToRasterizer.Binormal = mul(crossProd, WorldMatrix).xyz;

	return sendToRasterizer;
}