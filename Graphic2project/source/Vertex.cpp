#include"Vertex.h"
#include "Matrix.h"

Vertex::Vertex()
{

}

Vertex::Vertex(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

void Vertex::Setcolor(float _a, float _r, float _g, float _b)
{
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

Vertex Vertex::operator*(const Matrix& matrix)
{
	Vertex result;

	result.position[0] = (position[0] * matrix.matrix[0][0]) +
		(position[1] * matrix.matrix[1][0]) +
		(position[2] * matrix.matrix[2][0]) +
		(position[3] * matrix.matrix[3][0]);

	result.position[1] = (position[0] * matrix.matrix[0][1]) +
		(position[1] * matrix.matrix[1][1]) +
		(position[2] * matrix.matrix[2][1]) +
		(position[3] * matrix.matrix[3][1]);

	result.position[2] = (position[0] * matrix.matrix[0][2]) +
		(position[1] * matrix.matrix[1][2]) +
		(position[2] * matrix.matrix[2][2]) +
		(position[3] * matrix.matrix[3][2]);

	result.position[3] = (position[0] * matrix.matrix[0][3]) +
		(position[1] * matrix.matrix[1][3]) +
		(position[2] * matrix.matrix[2][3]) +
		(position[3] * matrix.matrix[3][3]);

	return result;
}

float Vertex::operator*(const Vertex& vertex)
{
	float result = 0;

	result += x * vertex.x;
	result += y * vertex.y;
	result += z * vertex.z;
	result += w * vertex.w;


	return result;
}

Vertex& Vertex::operator*=(const Matrix& matrix)
{
	float temp[4];

	/*
	x y z w  *	00 01 02 03
	10 11 12 13
	20 21 22 23
	30 31 32 33
	*/

	temp[0] = (position[0] * matrix.matrix[0][0]) +
		(position[1] * matrix.matrix[1][0]) +
		(position[2] * matrix.matrix[2][0]) +
		(position[3] * matrix.matrix[3][0]);

	temp[1] = (position[0] * matrix.matrix[0][1]) +
		(position[1] * matrix.matrix[1][1]) +
		(position[2] * matrix.matrix[2][1]) +
		(position[3] * matrix.matrix[3][1]);

	temp[2] = (position[0] * matrix.matrix[0][2]) +
		(position[1] * matrix.matrix[1][2]) +
		(position[2] * matrix.matrix[2][2]) +
		(position[3] * matrix.matrix[3][2]);

	temp[3] = (position[0] * matrix.matrix[0][3]) +
		(position[1] * matrix.matrix[1][3]) +
		(position[2] * matrix.matrix[2][3]) +
		(position[3] * matrix.matrix[3][3]);



	for (int i = 0; i < 4; i++)
	{
		position[i] = temp[i];
	}
	return *this;
}

float Vertex::operator*=(const Vertex& vertex)
{
	float result = 0;

	result += x * vertex.x;
	result += y * vertex.y;
	result += z * vertex.z;
	result += w * vertex.w;

	return result;
}

void Vertex::negate()
{
	position[0] = -position[0];
	position[1] = -position[1];
	position[2] = -position[2];
}


Vertex::~Vertex()
{
}
