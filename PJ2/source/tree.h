#pragma once

typedef enum _TreeType {
	Tree_0 = 0,
	Tree_1 = 1,
} TreeType;

typedef enum _LeafType {
	Leaf_0 = 0,
	Leaf_1 = 1,
} LeafType;

class Leaf {
public:
	LeafType	LType;
	double		radius = 30.0;

	Leaf() { }

	Leaf(LeafType type):LType(type){ }

	void updateType(LeafType type) {
		LType = type;
	}

	void updateRadius(double r) {
		radius = r;
	}

	~Leaf(){ }
};

class Trunk {
public:
	TreeType	TType;
	double		length = 10;
	double		lengthShrink = 0.75;
	double		radius = 0.75;
	double		radiusShrink = 0.65;
	double		leapX = 35;
	double		leapY = 30;
	double		leapZ = 30;

	Trunk() { }

	Trunk(TreeType type) :TType(type) { }

	void updateType(TreeType type) {
		TType = type;
	}

	void updateLength(double _length, double _lengthShrink) {
		length = _length;
		lengthShrink = _lengthShrink;
	}

	void updateRadius(double _radius, double _radiusShrink) {
		radius = _radius;
		radiusShrink = _radiusShrink;
	}

	void updateLeap(double _x, double _y, double _z) {
		leapX = _x;
		leapY = _y;
		leapZ = _z;
	}

	~Trunk() { }
};

class Tree {
public:
	TreeType	type;
	Trunk		trunk;
	Leaf		leaf;
	
	Tree() :type(Tree_0), trunk(Tree_0), leaf(Leaf_0) { }

	Tree(TreeType _type, LeafType _ltype) :type(_type), trunk(_type), leaf(_ltype) { }
	
	~Tree() {}
};