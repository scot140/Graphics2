#include "Matrix.h"
#include "Vertex.h"

Matrix::Matrix()
{
}

Matrix::Matrix(
	float e_00, float e_01, float e_02, float e_03,
	float e_10, float e_11, float e_12, float e_13,
	float e_20, float e_21, float e_22, float e_23,
	float e_30, float e_31, float e_32, float e_33)
{
	matrix[0][0] = e_00;
	matrix[0][1] = e_01;
	matrix[0][2] = e_02;
	matrix[0][3] = e_03;

	matrix[1][0] = e_10;
	matrix[1][1] = e_11;
	matrix[1][2] = e_12;
	matrix[1][3] = e_13;

	matrix[2][0] = e_20;
	matrix[2][1] = e_21;
	matrix[2][2] = e_22;
	matrix[2][3] = e_23;

	matrix[3][0] = e_30;
	matrix[3][1] = e_31;
	matrix[3][2] = e_32;
	matrix[3][3] = e_33;
}

void Matrix::MakeThisIdentity(void)
{
	float temp[4][4] = { 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			matrix[x][y] = temp[x][y];
		}
	}

};

void Matrix::TranslateMatrix(float x, float y, float z)
{
	MakeThisIdentity();

	matrix[3][0] = x;
	matrix[3][1] = y;
	matrix[3][2] = z;
}

void Matrix::SetTranslate(float x, float y, float z)
{
	matrix[3][0] = x;
	matrix[3][1] = y;
	matrix[3][2] = z;
}

void Matrix::MatrixInvertHelper()
{
	/*
	00 01 02 	00 01 02 03
	10 11 12 	10 11 12 13
	20 21 22		20 21 22 23
	30 31 32 33
	*/

	float temp;
	temp = matrix[1][0];
	matrix[1][0] = matrix[0][1];
	matrix[0][1] = temp;

	temp = matrix[2][0];
	matrix[2][0] = matrix[0][2];
	matrix[0][2] = temp;

	temp = matrix[1][2];
	matrix[1][2] = matrix[2][1];
	matrix[2][1] = temp;
}

Vertex Matrix::GetPos()
{
	Vertex temp;
	temp.x = matrix[3][0];
	temp.y = matrix[3][1];
	temp.z = matrix[3][2];
	temp.w = 1;
	return  temp;
}

Vertex Matrix::GetRow(int _row)
{
	//	00 01 02 03
	//	10 11 12 13
	//	20 21 22 23
	//	30 31 32 33

	Vertex row;

	for (int i = 0; i < 4; i++)
	{
		row[i] = matrix[_row][i];
	}

	return row;
}

Vertex Matrix::GetColumn(int _col)
{
	//	00 01 02 03
	//	10 11 12 13
	//	20 21 22 23
	//	30 31 32 33

	Vertex col;

	for (int i = 0; i < 4; i++)
	{
		col[i] = matrix[i][_col];
	}

	return col;
}

void Matrix::InverseOrthogonalAffinedMatrix()
{
	Matrix inverse = (*this);

	inverse.MatrixInvertHelper();

	TMatrix3x3 tempMatrix = inverse.GetMatrix3x3();

	Vertex newTranslation = inverse.GetPos();
	//multi
	MultiplyVertexByMatrix(newTranslation, tempMatrix);

	newTranslation.negate();

	inverse.SetTranslate(newTranslation.position[0], newTranslation.position[1], newTranslation.position[2]);

	(*this) = inverse;
}

Matrix::TMatrix3x3 Matrix::GetMatrix3x3()
{
	TMatrix3x3 temp;

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			temp.matrix[x][y] = matrix[x][y];
		}
	}
	return temp;

}

void Matrix::MultiplyVertexByMatrix(Vertex& vertex, Matrix::TMatrix3x3 matrix)
{
	float temp[4];

	/*
	0 1 2 * 00 01 02
	10 11 12
	20 21 22
	*/

	temp[0] = (vertex.position[0] * matrix.matrix[0][0]) +
		(vertex.position[1] * matrix.matrix[1][0]) +
		(vertex.position[2] * matrix.matrix[2][0]);

	temp[1] = (vertex.position[0] * matrix.matrix[0][1]) +
		(vertex.position[1] * matrix.matrix[1][1]) +
		(vertex.position[2] * matrix.matrix[2][1]);

	temp[2] = (vertex.position[0] * matrix.matrix[0][2]) +
		(vertex.position[1] * matrix.matrix[1][2]) +
		(vertex.position[2] * matrix.matrix[2][2]);

	temp[3] = 1;

	for (int i = 0; i < 4; i++)
	{
		vertex.position[i] = temp[i];
	}
}

Matrix Matrix::operator*(const Matrix& _matrix)
{

	float temp[4][4];
	/*


	00 01 02 03		00 01 02 03
	10 11 12 13		10 11 12 13
	20 21 22 23		20 21 22 23
	30 31 32 33		30 31 32 33

	*/

#pragma region first row
	temp[0][0] = (matrix[0][0] * _matrix.matrix[0][0]) +
		(matrix[0][1] * _matrix.matrix[1][0]) +
		(matrix[0][2] * _matrix.matrix[2][0]) +
		(matrix[0][3] * _matrix.matrix[3][0]);

	temp[0][1] = (matrix[0][0] * _matrix.matrix[0][1]) +
		(matrix[0][1] * _matrix.matrix[1][1]) +
		(matrix[0][2] * _matrix.matrix[2][1]) +
		(matrix[0][3] * _matrix.matrix[3][1]);

	temp[0][2] = (matrix[0][0] * _matrix.matrix[0][2]) +
		(matrix[0][1] * _matrix.matrix[1][2]) +
		(matrix[0][2] * _matrix.matrix[2][2]) +
		(matrix[0][3] * _matrix.matrix[3][2]);

	temp[0][3] = (matrix[0][0] * _matrix.matrix[0][3]) +
		(matrix[0][1] * _matrix.matrix[1][3]) +
		(matrix[0][2] * _matrix.matrix[2][3]) +
		(matrix[0][3] * _matrix.matrix[3][3]);
#pragma endregion

#pragma region second row
	temp[1][0] = (matrix[1][0] * _matrix.matrix[0][0]) +
		(matrix[1][1] * _matrix.matrix[1][0]) +
		(matrix[1][2] * _matrix.matrix[2][0]) +
		(matrix[1][3] * _matrix.matrix[3][0]);
	temp[1][1] = (matrix[1][0] * _matrix.matrix[0][1]) +
		(matrix[1][1] * _matrix.matrix[1][1]) +
		(matrix[1][2] * _matrix.matrix[2][1]) +
		(matrix[1][3] * _matrix.matrix[3][1]);
	temp[1][2] =
		(matrix[1][0] * _matrix.matrix[0][2]) +
		(matrix[1][1] * _matrix.matrix[1][2]) +
		(matrix[1][2] * _matrix.matrix[2][2]) +
		(matrix[1][3] * _matrix.matrix[3][2]);
	temp[1][3] =
		(matrix[1][0] * _matrix.matrix[0][3]) +
		(matrix[1][1] * _matrix.matrix[1][3]) +
		(matrix[1][2] * _matrix.matrix[2][3]) +
		(matrix[1][3] * _matrix.matrix[3][3]);
#pragma endregion

#pragma region thrid row
	temp[2][0] =
		(matrix[2][0] * _matrix.matrix[0][0]) +
		(matrix[2][1] * _matrix.matrix[1][0]) +
		(matrix[2][2] * _matrix.matrix[2][0]) +
		(matrix[2][3] * _matrix.matrix[3][0]);


	temp[2][1] =
		(matrix[2][0] * _matrix.matrix[0][1]) +
		(matrix[2][1] * _matrix.matrix[1][1]) +
		(matrix[2][2] * _matrix.matrix[2][1]) +
		(matrix[2][3] * _matrix.matrix[3][1]);

	temp[2][2] =
		(matrix[2][0] * _matrix.matrix[0][2]) +
		(matrix[2][1] * _matrix.matrix[1][2]) +
		(matrix[2][2] * _matrix.matrix[2][2]) +
		(matrix[2][3] * _matrix.matrix[3][2]);

	temp[2][3] =
		(matrix[2][0] * _matrix.matrix[0][3]) +
		(matrix[2][1] * _matrix.matrix[1][3]) +
		(matrix[2][2] * _matrix.matrix[2][3]) +
		(matrix[2][3] * _matrix.matrix[3][3]);
#pragma endregion

#pragma region Fourth row
	temp[3][0] =
		(matrix[3][0] * _matrix.matrix[0][0]) +
		(matrix[3][1] * _matrix.matrix[1][0]) +
		(matrix[3][2] * _matrix.matrix[2][0]) +
		(matrix[3][3] * _matrix.matrix[3][0]);


	temp[3][1] =
		(matrix[3][0] * _matrix.matrix[0][1]) +
		(matrix[3][1] * _matrix.matrix[1][1]) +
		(matrix[3][2] * _matrix.matrix[2][1]) +
		(matrix[3][3] * _matrix.matrix[3][1]);

	temp[3][2] =
		(matrix[3][0] * _matrix.matrix[0][2]) +
		(matrix[3][1] * _matrix.matrix[1][2]) +
		(matrix[3][2] * _matrix.matrix[2][2]) +
		(matrix[3][3] * _matrix.matrix[3][2]);

	temp[3][3] =
		(matrix[3][0] * _matrix.matrix[0][3]) +
		(matrix[3][1] * _matrix.matrix[1][3]) +
		(matrix[3][2] * _matrix.matrix[2][3]) +
		(matrix[3][3] * _matrix.matrix[3][3]);
#pragma endregion

	Matrix newMatrix;

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			newMatrix.matrix[x][y] = temp[x][y];

		}
	}
	return newMatrix;
}

Matrix& Matrix::operator*=(const Matrix& matrix)
{

	float temp[4][4];
	/*


	00 01 02 03		00 01 02 03
	10 11 12 13		10 11 12 13
	20 21 22 23		20 21 22 23
	30 31 32 33		30 31 32 33

	*/

#pragma region first row
	temp[0][0] = (this->matrix[0][0] * matrix.matrix[0][0]) +
		(this->matrix[0][1] * matrix.matrix[1][0]) +
		(this->matrix[0][2] * matrix.matrix[2][0]) +
		(this->matrix[0][3] * matrix.matrix[3][0]);

	temp[0][1] = (this->matrix[0][0] * matrix.matrix[0][1]) +
		(this->matrix[0][1] * matrix.matrix[1][1]) +
		(this->matrix[0][2] * matrix.matrix[2][1]) +
		(this->matrix[0][3] * matrix.matrix[3][1]);

	temp[0][2] = (this->matrix[0][0] * matrix.matrix[0][2]) +
		(this->matrix[0][1] * matrix.matrix[1][2]) +
		(this->matrix[0][2] * matrix.matrix[2][2]) +
		(this->matrix[0][3] * matrix.matrix[3][2]);

	temp[0][3] = (this->matrix[0][0] * matrix.matrix[0][3]) +
		(this->matrix[0][1] * matrix.matrix[1][3]) +
		(this->matrix[0][2] * matrix.matrix[2][3]) +
		(this->matrix[0][3] * matrix.matrix[3][3]);
#pragma endregion

#pragma region second row
	temp[1][0] = (this->matrix[1][0] * matrix.matrix[0][0]) +
		(this->matrix[1][1] * matrix.matrix[1][0]) +
		(this->matrix[1][2] * matrix.matrix[2][0]) +
		(this->matrix[1][3] * matrix.matrix[3][0]);


	temp[1][1] = (this->matrix[1][0] * matrix.matrix[0][1]) +
		(this->matrix[1][1] * matrix.matrix[1][1]) +
		(this->matrix[1][2] * matrix.matrix[2][1]) +
		(this->matrix[1][3] * matrix.matrix[3][1]);

	temp[1][2] = (this->matrix[1][0] * matrix.matrix[0][2]) +
		(this->matrix[1][1] * matrix.matrix[1][2]) +
		(this->matrix[1][2] * matrix.matrix[2][2]) +
		(this->matrix[1][3] * matrix.matrix[3][2]);

	temp[1][3] = (this->matrix[1][0] * matrix.matrix[0][3]) +
		(this->matrix[1][1] * matrix.matrix[1][3]) +
		(this->matrix[1][2] * matrix.matrix[2][3]) +
		(this->matrix[1][3] * matrix.matrix[3][3]);
#pragma endregion

#pragma region thrid row
	temp[2][0] = (this->matrix[2][0] * matrix.matrix[0][0]) +
		(this->matrix[2][1] * matrix.matrix[1][0]) +
		(this->matrix[2][2] * matrix.matrix[2][0]) +
		(this->matrix[2][3] * matrix.matrix[3][0]);


	temp[2][1] = (this->matrix[2][0] * matrix.matrix[0][1]) +
		(this->matrix[2][1] * matrix.matrix[1][1]) +
		(this->matrix[2][2] * matrix.matrix[2][1]) +
		(this->matrix[2][3] * matrix.matrix[3][1]);

	temp[2][2] = (this->matrix[2][0] * matrix.matrix[0][2]) +
		(this->matrix[2][1] * matrix.matrix[1][2]) +
		(this->matrix[2][2] * matrix.matrix[2][2]) +
		(this->matrix[2][3] * matrix.matrix[3][2]);

	temp[2][3] = (this->matrix[2][0] * matrix.matrix[0][3]) +
		(this->matrix[2][1] * matrix.matrix[1][3]) +
		(this->matrix[2][2] * matrix.matrix[2][3]) +
		(this->matrix[2][3] * matrix.matrix[3][3]);
#pragma endregion

#pragma region Fourth row
	temp[3][0] = (this->matrix[3][0] * matrix.matrix[0][0]) +
		(this->matrix[3][1] * matrix.matrix[1][0]) +
		(this->matrix[3][2] * matrix.matrix[2][0]) +
		(this->matrix[3][3] * matrix.matrix[3][0]);


	temp[3][1] = (this->matrix[3][0] * matrix.matrix[0][1]) +
		(this->matrix[3][1] * matrix.matrix[1][1]) +
		(this->matrix[3][2] * matrix.matrix[2][1]) +
		(this->matrix[3][3] * matrix.matrix[3][1]);

	temp[3][2] = (this->matrix[3][0] * matrix.matrix[0][2]) +
		(this->matrix[3][1] * matrix.matrix[1][2]) +
		(this->matrix[3][2] * matrix.matrix[2][2]) +
		(this->matrix[3][3] * matrix.matrix[3][2]);

	temp[3][3] = (this->matrix[3][0] * matrix.matrix[0][3]) +
		(this->matrix[3][1] * matrix.matrix[1][3]) +
		(this->matrix[3][2] * matrix.matrix[2][3]) +
		(this->matrix[3][3] * matrix.matrix[3][3]);
#pragma endregion


	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			this->matrix[x][y] = temp[x][y];

		}
	}
	return *this;
}

Vertex Matrix::operator[](const unsigned int row)
{
	return GetRow(row);
}

Matrix::~Matrix()
{
}
