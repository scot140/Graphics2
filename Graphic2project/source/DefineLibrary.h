#pragma once

#define RASTER_WIDTH	500
#define RASTER_HEIGHT	500
#define ZFAR			100
#define ZNEAR			0.1f
#define FOV				65
#define ASPECT_RATIO  (RASTER_WIDTH/RASTER_HEIGHT)
#define RASTER_NUMPIXEL (RASTER_WIDTH * RASTER_HEIGHT)



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

