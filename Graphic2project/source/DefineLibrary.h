#pragma once

#define RASTER_WIDTH	800
#define RASTER_HEIGHT	800
#define ZFAR			100
#define ZNEAR			0.1f
#define FOV				65
#define ASPECT_RATIO  (RASTER_WIDTH/RASTER_HEIGHT)
#define RASTER_NUMPIXEL (RASTER_WIDTH * RASTER_HEIGHT)
#define ALIGN_REGISTER _declspec(align(16))
#define ZERO_OUT(x) ZeroMemory(&x,sizeof(x));

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <DirectXMath.h>
using namespace DirectX;

struct Object
{
	XMFLOAT4X4 m_mxConstMatrix;
};

struct Scene
{
	XMFLOAT4X4 matrix_sceneCamera;
	XMFLOAT4X4 matrix_Projection;
};

struct INPUT_VERTEX
{
	XMFLOAT4 pos;
	XMFLOAT4 col;
	XMFLOAT3 normals;
	XMFLOAT2 uv;
	XMFLOAT4 tangents;
	XMFLOAT3 padding;
};

struct InstanceType
{
	XMFLOAT4 pos;
};

struct ANIMATION
{
	float frame = 0;
	float maxFrame = 0;
	float width;
	float padding;
};

struct LIGHTING
{
	float padding;
	XMFLOAT3 dir;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

struct PtLight
{
	float range;
	float power;
	XMFLOAT2 padding;
	XMFLOAT4 color;
	XMFLOAT4 pos;
};

struct SptLight
{
	float power;
	XMFLOAT3 coneDir;
	float coneWidth; // use Radians please do not forget
	XMFLOAT3 padding;
	XMFLOAT4 pos;
	XMFLOAT4 color;
};
