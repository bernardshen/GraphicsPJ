#pragma once
#include <glut.h>
#include "grammar.h"
#include "tree.h"
#include "transformation.h"


class State {
public:
	Node pos;
	Node dir;
	State() {

	}
};

class TrunkPosition {
public:
	Node pos[2];
	double radius;
	TrunkPosition() {

	}
};

class LeafPosition {
public:
	Node pos[2];
	double radius;
	LeafPosition() {

	}
};

class LSystem {
public:
	int		stackpointer;
	double	dx, dy, dz;
	double	length;
	double	lengthFactor;
	double	radius;
	double	radiusFactor;
	State	curState;
	double	leafRadius;
	Grammar grammar;

	vector<TrunkPosition>	trunks;
	vector<LeafPosition>	leaves;

	LSystem(Tree tree);
	void clearAll();
	void initGrammar();
	void generateFractal();
};