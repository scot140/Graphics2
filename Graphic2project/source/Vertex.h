#pragma once

class Matrix;

class Vertex
{

public:
	//point
	union
	{
		struct
		{
			float x, y, z, w;
		};
		float position[4];

	};
	//color
	union
	{
		struct
		{
			float r, g, b, a;
		};
		float color[4];
	};
	//UV
	union
	{
		struct
		{
			float u, v;
		};
		float uv[2];
	};
	//constructor
	Vertex();

	Vertex(float x, float y, float z, float w = 1);

	void Setcolor(float _a = 1, float _r = 1, float _g = 1, float _b = 1);

	void negate();


	float operator*(const Vertex& matrix);
	Vertex operator*(const Matrix& matrix);
	Vertex& operator*=(const Matrix& matrix);
	
	float operator*=(const Vertex& matrix);
	float& operator[](const unsigned int col)
	{
		return position[col];
	}

	//destructor
	~Vertex();
};

