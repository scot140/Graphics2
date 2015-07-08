#pragma once

class Vertex;

class Matrix
{


	XMFLOAT3X3 GetMatrix3x3();

public:

	Matrix();
	Matrix(
		float e_00, float e_01, float e_02, float e_03,
		float e_10, float e_11, float e_12, float e_13,
		float e_20, float e_21, float e_22, float e_23,
		float e_30, float e_31, float e_32, float e_33);

	float matrix[4][4];

	void MakeThisIdentity(void);

	void TranslateMatrix(float x = 0, float y = 0, float z = 0);

	void SetTranslate(float x = 0, float y = 0, float z = 0);


	Vertex GetPos();

	Vertex GetRow(int _row);

	Vertex GetColumn(int _col);

	void InverseOrthogonalAffinedMatrix();

	Matrix operator*(const Matrix& matrix);

	Vertex operator[](const unsigned int row);

	Matrix& operator*=(const Matrix& matrix);

	~Matrix();


};

