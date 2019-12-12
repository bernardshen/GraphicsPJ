#pragma once


class Node {
public:
	float x, y, z, w;

	Node() :x(0), y(0), z(0), w(1) { }

	Node(float _x, float _y, float _z) :x(_x), y(_y), z(_z), w(1) { }
};

class Transformation {
public:
	Node	POld;
	float	T[4][4];

	Transformation() { }

	void set(Node point);
	void Identity();
	Node Translate(float, float, float);
	Node Scale(float, float, float);
	Node Rotate(char, double);
	void MultiMatrix();
};