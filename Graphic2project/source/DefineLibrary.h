#pragma once

#define RASTER_WIDTH	800
#define RASTER_HEIGHT	800
#define ZFAR			100
#define ZNEAR			0.1f
#define FOV				65
#define ASPECT_RATIO  (RASTER_WIDTH/RASTER_HEIGHT)
#define RASTER_NUMPIXEL (RASTER_WIDTH * RASTER_HEIGHT)

#define ZERO_OUT(x) ZeroMemory(&x,sizeof(x));

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

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
	XMFLOAT3 dir;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	float padding;
};

struct PtLight
{
	XMFLOAT4 pos;
	float range;
	float power;
	XMFLOAT4 color;
	XMFLOAT4 diffuse;
	XMFLOAT2 padding;
};

