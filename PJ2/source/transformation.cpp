#include "transformation.h"
#include <iostream>
#include <math.h>

using namespace std;
const double PI = 3.1415926;

void Transformation::set(Node p) {
	POld = p;
}

void Transformation::Identity() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j)
				T[i][j] = 1;
			else
				T[i][j] = 0;
		}
	}
}

Node Transformation::Translate(float tx, float ty, float tz) {
	Identity();
	T[3][0] = tx;
	T[3][1] = ty;
	T[3][2] = tz;
	MultiMatrix();
	return POld;
}

Node Transformation::Scale(float sx, float sy, float sz) {
	Identity();
	T[0][0] = sx;
	T[1][1] = sy;
	T[2][2] = sz;
	MultiMatrix();
	return POld;
}

Node Transformation::Rotate(char shaft, double beta) {
	Identity();
	double rad = (beta * PI) / 180;
	switch (shaft)
	{
	case 'X':
		T[0][0] = 1;
		T[1][1] = cos(rad); T[1][2] = sin(rad);
		T[2][1] = -sin(rad); T[2][2] = cos(rad);
		break;
	case 'Y':
		T[0][0] = cos(rad); T[0][2] = -sin(rad);
		T[1][1] = 1;
		T[2][0] = sin(rad); T[2][2] = cos(rad);
		break;
	case 'Z':
		T[0][0] = cos(rad); T[0][1] = sin(rad);
		T[1][0] = -sin(rad); T[1][1] = cos(rad);
		T[2][2] = 1;
	}
	MultiMatrix();
	return POld;
}

void Transformation::MultiMatrix()
{
	Node PNew = POld;

	POld.x = PNew.x * T[0][0] + PNew.y * T[1][0] + PNew.z * T[2][0] + PNew.w * T[3][0];
	POld.y = PNew.x * T[0][1] + PNew.y * T[1][1] + PNew.z * T[2][1] + PNew.w * T[3][1];
	POld.z = PNew.x * T[0][2] + PNew.y * T[1][2] + PNew.z * T[2][2] + PNew.w * T[3][2];
	POld.w = PNew.x * T[0][3] + PNew.y * T[1][3] + PNew.z * T[2][3] + PNew.w * T[3][3];
}