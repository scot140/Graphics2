#pragma once
#include "DefineLibrary.h"

#include <Windows.h>

#define DegToRad (M_PI/180.0f)

#define RadToDeg (180.0f/M_PI)
//Funtion Headers


//Structures
struct TPoint
{
	unsigned int x = 0;
	unsigned int y = 0;
};
//TFPoint(Point that uses floating point variables)
struct TFPoint
{
	float x = 0;
	float y = 0;
};

struct TLine
{
	TPoint startPos;
	TPoint endPos;
};

//Holds an unsigned Width and Height
struct TSize
{
	unsigned int m_nWidth = 0;
	unsigned int m_nHeight = 0;

	unsigned int Area()
	{
		return m_nWidth *m_nHeight;
	}
};

struct TRectangle
{
	unsigned int m_nTop = 0;
	unsigned int m_nLeft = 0;
	unsigned int m_nRight = 0;
	unsigned int m_nBottom = 0;
};

struct TVertex
{
	union
	{
		struct
		{
			float x, y, z, w;
		};
		float xyzw[4];
	};
	unsigned int color;

	float uv[2];

	TVertex(float x = 0, float y = 0, float z = 0, float w = 1, unsigned int _color = 0xFFFFFFFF)
	{
		xyzw[0] = x;
		xyzw[1] = y;
		xyzw[2] = z;
		xyzw[3] = w;
		color = _color;
	}

	void negate()
	{
		xyzw[0] = -xyzw[0];
		xyzw[1] = -xyzw[1];
		xyzw[2] = -xyzw[2];
	}

	//Apply this after you have add your projection view
	float LinearZ_Reciprocal()
	{
		return 1 / xyzw[3];
	}
};

//TFLine(Line that uses Vvertex variables)
struct TFLine
{
	TVertex startVert;
	TVertex endVert;
};

struct TTriangle
{
	TVertex VertexA;
	TVertex VertexB;
	TVertex VertexC;

	TVertex* GetAllVerts()
	{
		TVertex Verts[3];

		Verts[0] = VertexA;
		Verts[1] = VertexB;
		Verts[2] = VertexC;

		return &Verts[0];
	}

	void SetColorToAll(unsigned int _color)
	{
		VertexA.color = _color;
		VertexB.color = _color;
		VertexC.color = _color;
	}

	void SetUVs(float _uvA[], float _uvB[], float _uvC[])
	{
		memcpy_s(VertexA.uv, sizeof(VertexA.uv), _uvA, sizeof(float) * 2);
		memcpy_s(VertexB.uv, sizeof(VertexA.uv), _uvB, sizeof(float) * 2);
		memcpy_s(VertexC.uv, sizeof(VertexA.uv), _uvC, sizeof(float) * 2);
	}
};

float ImplicitLineEquation(float x, float y, TLine line)
{
	float Yval = (((float)line.startPos.y - line.endPos.y)*x);
	float Xval = (((float)line.endPos.x - (float)line.startPos.x)*y);
	float X1byY2 = (line.startPos.x*(float)line.endPos.y);
	float X2byY1 = (line.endPos.x* (float)line.startPos.y);
	float temp = Yval + Xval + X1byY2 - X2byY1;
	return temp;
}

unsigned int Lerp(unsigned int start, unsigned int end, float ratio)
{
	float temp = 0;

	float minus = (float)end - start;

	float multiply = (minus * ratio);

	temp = multiply + (float)start;

	return (unsigned int)temp;
}

float Lerp(float start, float end, float ratio)
{
	float temp = 0;

	float minus = end - start;

	float multiply = (minus * ratio);

	temp = multiply + start;

	return temp;
}

unsigned int ColorBlending(unsigned int startColor, unsigned int endColor, float _ratio)
{
	//ratio
	//end / 255
	float ratio = _ratio;

	//Starts
	unsigned int m_unStartAlpha = (startColor & 0xFF000000) >> 24;

	unsigned int m_unStartRed = (startColor & 0x00FF0000) >> 16;

	unsigned int m_unStartGreen = (startColor & 0x0000FF00) >> 8;

	unsigned int m_unStartBlue = (startColor & 0x000000FF);

	//Ends
	unsigned int m_unEndAlpha = (endColor & 0xFF000000) >> 24;

	unsigned int m_unEndRed = (endColor & 0x00FF0000) >> 16;

	unsigned int m_unEndGreen = (endColor & 0x0000FF00) >> 8;

	unsigned int m_unEndBlue = (endColor & 0x000000FF);
	//Blends

	//ratio
	//Color blend Formula : ((int)end - (int)start) * ratio ("end / 0xFFFFFFFF") + start

	//ratio = (float)m_unEndAlpha / 255.0f;

	int minus = (int)m_unEndAlpha - (int)m_unStartAlpha;
	unsigned int m_unAlphaBlend = unsigned int(minus * ratio) + m_unStartAlpha;

	/*ratio = (float)m_unEndRed / 255.0f;*/
	minus = (int)m_unEndRed - (int)m_unStartRed;

	unsigned int m_unRedBlend = (unsigned int)(minus * ratio) + m_unStartRed;

	/*ratio = (float)m_unEndGreen / 255.0f;*/
	minus = (int)m_unEndGreen - (int)m_unStartGreen;

	unsigned int m_unGreenBlend = (unsigned int)(minus * ratio) + m_unStartGreen;

	/*ratio = (float)m_unEndBlue / 255.0f;*/
	minus = (int)m_unEndBlue - (int)m_unStartBlue;

	unsigned int m_unBlueBlend = (unsigned int)(minus * ratio) + m_unStartBlue;

	//Result
	unsigned int m_unColorBlend = 0;

	m_unColorBlend |= (m_unAlphaBlend << 24);
	m_unColorBlend |= (m_unRedBlend << 16);
	m_unColorBlend |= (m_unGreenBlend << 8);
	m_unColorBlend |= m_unBlueBlend;

	return m_unColorBlend;
}

//OverWrites the Matrix on the Left Hand side of the function parameter
float DegreesToRadians(float deg)
{
	float temp = deg * (float)(DegToRad);
	return temp;
}

float RadiansToDegrees(float rad)
{
	float temp = rad * (float)(RadToDeg);
	return temp;
}

TPoint NDCtoScreen(TVertex NDCpos, TSize screen)
{
	unsigned int halfWidth = screen.m_nWidth >> 1;
	unsigned int halfHeight = screen.m_nHeight >> 1;

	TPoint temp;
	temp.x = (unsigned int)((NDCpos.xyzw[0] * halfWidth) + halfWidth);

	float te = (-NDCpos.xyzw[1] * halfHeight);
	te = (te + halfHeight);
	temp.y = (unsigned int)te;

	return temp;
}

TFPoint ScreenToNDC(unsigned int x, unsigned int y, TSize screen)
{
	unsigned int halfWidth = screen.m_nWidth >> 1;
	unsigned int halfHeight = screen.m_nHeight >> 1;

	TFPoint temp;

	temp.x = ((float)x - halfWidth) / (float)halfWidth;
	temp.y = (-(float)y + halfHeight) / (float)halfHeight;

	return temp;
}

TFPoint ScreenToNDC(unsigned int x, unsigned int y, int width, int height)
{
	unsigned int halfWidth = width >> 1;
	unsigned int halfHeight = height >> 1;

	TFPoint temp;

	temp.x = ((float)x - halfWidth) / (float)halfWidth;
	temp.y = (-(float)y + halfHeight) / (float)halfHeight;

	return temp;
}

unsigned int Convert2Dto1D(const unsigned int _x, const unsigned int _y, const unsigned int _width)
{
	return (_y * _width) + _x;
}

Matrix CreateProjectionMatrix()
{
	float Yscale = 1 / tan(.5f* DegreesToRadians(FOV));

	float Xscale = Yscale * ASPECT_RATIO;

	Matrix matrix;
	matrix.MakeThisIdentity();

	matrix.matrix[2][3] = 1.0f;
	matrix.matrix[0][0] = Xscale;
	matrix.matrix[1][1] = Yscale;
	matrix.matrix[2][2] = (float)ZFAR / (float)(ZFAR - ZNEAR);
	matrix.matrix[3][2] = (float)((ZFAR*ZNEAR) * -1) / (float)(ZFAR - ZNEAR);
	matrix.matrix[3][3] = 0;

	return matrix;
}

Matrix RotationMatrixOnZ_Axiz(float angle)
{
	Matrix temp(cos(angle), -sin(angle), 0, 0,
		sin(angle), cos(angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	return temp;
}

Matrix RotationMatrixOnY_Axis(float angle)
{
	Matrix temp = {
		cos(angle), 0, sin(angle), 0,
		0, 1, 0, 0,
		-sin(angle), 0, cos(angle), 0,
		0, 0, 0, 1 };



	return temp;
}

Matrix RotationMatrixOnX_Axis(float angle)
{
	Matrix temp = {
		1, 0, 0, 0,
		0, cos(angle), -sin(angle), 0,
		0, sin(angle), cos(angle), 0,
		0, 0, 0, 1

	};

	return temp;
}

TPoint MIN_VERT(TVertex* _vert, unsigned int max)
{
	TPoint result;

	TSize Screen;
	Screen.m_nWidth = RASTER_WIDTH;
	Screen.m_nHeight = RASTER_HEIGHT;

	TPoint points[3];
	result = points[0] = NDCtoScreen(_vert[0], Screen);
	points[1] = NDCtoScreen(_vert[1], Screen);
	points[2] = NDCtoScreen(_vert[2], Screen);


	for (unsigned int i = 0; i < max; i++)
	{
		result.x = (points[i].x < result.x) ? points[i].x : result.x;

		result.y = (points[i].y < result.y) ? points[i].y : result.y;
	}

	/*
	*/

	return result;
}

TPoint MAX_VERT(TVertex* _vert, unsigned int max)
{
	TPoint result;

	TSize Screen;
	Screen.m_nWidth = RASTER_WIDTH;
	Screen.m_nHeight = RASTER_HEIGHT;

	TPoint points[3];
	points[0] = NDCtoScreen(_vert[0], Screen);
	points[1] = NDCtoScreen(_vert[1], Screen);
	points[2] = NDCtoScreen(_vert[2], Screen);


	for (unsigned int i = 0; i < max; i++)
	{
		result.x = (points[i].x > result.x) ? points[i].x : result.x;

		result.y = (points[i].y > result.y) ? points[i].y : result.y;
	}

	return result;
}

//Takes in a TFLine(Line that uses Vertex variables)
float ImplicitLineEquation(float x, float y, TFLine line)
{
	float temp = ((line.startVert.xyzw[1] - line.endVert.xyzw[1])*x) +
		((line.endVert.xyzw[0] - line.startVert.xyzw[0])*y) +
		(line.startVert.xyzw[0] * line.endVert.xyzw[1]) -
		(line.startVert.xyzw[1] * line.endVert.xyzw[0]);
	return temp;
}

//Convert vertex to a TFPoint 
TFPoint VertToPoint(TVertex* _vert)
{
	TFPoint temp;
	temp.x = _vert->xyzw[0];
	temp.y = _vert->xyzw[1];

	return temp;
}

//Takes in unsigned int x unsigned int y and the array of lines for TRIANGLES
float* FindBarycentric(unsigned int x, unsigned int y, TLine* Lines)
{
	//Implicit Line for the Big var
	float Bigtemp[3];


	Bigtemp[0] = ImplicitLineEquation((float)Lines[1].startPos.x, (float)Lines[1].startPos.y, Lines[0]);

	Bigtemp[1] = ImplicitLineEquation((float)Lines[2].startPos.x, (float)Lines[2].startPos.y, Lines[1]);

	Bigtemp[2] = ImplicitLineEquation((float)Lines[0].startPos.x, (float)Lines[0].startPos.y, Lines[2]);



	//Implicit Line for the little var
	float Littletemp[3];

	for (int i = 0; i < 3; i++)
	{
		Littletemp[i] = ImplicitLineEquation((float)x, (float)y, Lines[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		Littletemp[i] = Littletemp[i] / Bigtemp[i];
	}

	return &Littletemp[0];
}

float* VertexBarycentric(unsigned int x, unsigned int y, TFLine* Lines)
{
	//Implicit Line for the Big var
	float Bigtemp[3];

	Bigtemp[0] = ImplicitLineEquation((float)Lines[1].startVert.xyzw[0], (float)Lines[1].startVert.xyzw[1], Lines[0]);

	Bigtemp[1] = ImplicitLineEquation((float)Lines[2].startVert.xyzw[0], (float)Lines[2].startVert.xyzw[1], Lines[1]);

	Bigtemp[2] = ImplicitLineEquation((float)Lines[0].startVert.xyzw[0], (float)Lines[0].startVert.xyzw[1], Lines[2]);

	//Implicit Line for the little var
	float Littletemp[3];

	for (int i = 0; i < 3; i++)
	{
		Littletemp[i] = ImplicitLineEquation((float)x, (float)y, Lines[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		Littletemp[i] = Littletemp[i] / Bigtemp[i];
	}

	return &Littletemp[0];
}

float InterpolateForZ(float bya[], TVertex verts[])
{

	float Z = (verts[1].xyzw[2] * bya[0]) +
		(verts[2].xyzw[2] * bya[1]) +
		(verts[0].xyzw[2] * bya[2]);

	return Z;
}

float BarycentricInterpolation(float bya[], float A, float B, float C)
{

	return	B * bya[0] +
		C * bya[1] +
		A * bya[2];

}

unsigned int BGRAtoARGB(unsigned int _color)
{
	//storing the passed in variable
	unsigned int blue = (_color >> 24) & 0x000000FF;
	unsigned int green = (_color >> 16) & 0x000000FF;
	unsigned int red = (_color >> 8) & 0x000000FF;
	unsigned int alpha = _color & 0x000000FF;

	//flipping it to ARGB
	unsigned int temp = 0;
	temp |= (alpha << 24);
	temp |= (red << 16);
	temp |= (green << 8);
	temp |= blue;

	return temp;
}
