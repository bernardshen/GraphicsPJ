#include "LSystem.h"

GLfloat PI = 3.14;
Transformation	trans;

LSystem::LSystem(Tree tree) {
	stackpointer = 0;
	dx = tree.trunk.leapX;
	dy = tree.trunk.leapY;
	dz = tree.trunk.leapZ;
	length = tree.trunk.length;

	lengthFactor = tree.trunk.lengthShrink;
	radius = tree.trunk.radius;
	radiusFactor = tree.trunk.radiusShrink;
	leafRadius = tree.leaf.radius;
	curState = {};
}

void LSystem::clearAll() {
	grammar.clear();
	trunks.clear();
	leaves.clear();
}

void LSystem::initGrammar() {
	grammar.Iteration();
}

void LSystem::generateFractal() {
	trunks.clear();
	leaves.clear();
	curState.pos = Node(0, 0, 0);
	curState.dir = Node(0, 1, 0);

	State stack[3000];

	for (int i = 0; i < 3000; i++) {
		stack[i].pos.x = 0;
		stack[i].pos.y = 0;
		stack[i].pos.z = 0;
		stack[i].dir.x = 0;
		stack[i].dir.y = 0;
		stack[i].dir.z = 0;
	}
	size_t i = 0;
	while (i < grammar.getRule().length()) {
		TrunkPosition	tempTrunk;
		LeafPosition	tempLeaf;
		switch (grammar.getRule()[i]) {
		case 'F':
			tempTrunk.pos[0] = curState.pos;
			curState.pos.x += length * curState.dir.x;
			curState.pos.y += length * curState.dir.y;
			curState.pos.z += length * curState.dir.z;
			tempTrunk.pos[1] = curState.pos;

			tempTrunk.radius = radius;
			trunks.push_back(tempTrunk);
			break;
		case 'X':
			tempLeaf.pos[0] = curState.pos;
			tempTrunk.pos[0] = curState.pos;
			curState.pos.x += length * curState.dir.x;
			curState.pos.y += length * curState.dir.y;
			curState.pos.z += length * curState.dir.z;
			tempTrunk.pos[1] = curState.pos;
			tempLeaf.pos[1] = curState.pos;
			tempLeaf.radius = 6;
			tempTrunk.radius = radius;
			trunks.push_back(tempTrunk);
			leaves.push_back(tempLeaf);
			break;
		case 'A':
			length = length * lengthFactor;
			radius = radius * radiusFactor;
			break;
		case 'B':
			length = length / lengthFactor;
			radius = radius / radiusFactor;
			break;
		case '[':
			stack[stackpointer++] = curState;
			break;
		case ']':
			curState = stack[--stackpointer];
			break;
		case '+':
			trans.set(curState.dir);
			curState.dir = trans.Rotate('X', dx);
			break;
		case '-':
			trans.set(curState.dir);
			curState.dir = trans.Rotate('X', -dx);
			break;
		case '*':
			trans.set(curState.dir);
			curState.dir = trans.Rotate('Y', dy);
			break;
		case '/':
			trans.set(curState.dir);
			curState.dir = trans.Rotate('Z', -dy);
			break;
		case '&':
			trans.set(curState.dir);
			curState.dir = trans.Rotate('Z', dz);
			break;
		case '%':
			trans.set(curState.dir);
			curState.dir = trans.Rotate('Z', -dz);
			break;
		default:
			;
		}
		i++;
	}
}