#pragma once

#define RASTER_WIDTH	500
#define RASTER_HEIGHT	500
#define ZFAR			100
#define ZNEAR			.1
#define FOV				65
#define ASPECT_RATIO  (RASTER_WIDTH/RASTER_HEIGHT)
#define RASTER_NUMPIXEL (RASTER_WIDTH * RASTER_HEIGHT)


#define _USE_MATH_DEFINES
#include <math.h>
#include "Matrix.h"
#include "Vertex.h"
