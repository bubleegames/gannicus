#ifndef ___force
#define ___force
#include "player.h"

class force {
public:
	force() : x(0), y(0), type(0), radius(0), eventHorizon(0), effectCode(3), origin(nullptr), check(nullptr), grip(0) {}
	int x, y; //X-Yvalue. For globals this is only influenced by facingness, for local it is attractive force (negative for repulsive) based on mean Y
	int length; //How long the force holds
	int ID; //Who the force affects
	unsigned int type:2; //Type check variable. Type 0 is global, type 1 is linear decay, type 2 is half-life, and type 3 is a flat cut-off.
	int posX, posY; //Used to keep track of where local forces actually are. Not set by the move itself, but used by the game later.
	int radius;/*The radius at which the vector decay. Irrelevant (unchecked) for type 0, is the distance at which the vector loses one
				*from its absolute value in type 1, the half-life point for type 2, and the cut-off point for type 3.
				*/
	int eventHorizon;
	int effectCode;
	instance * origin;
	action * check;
	int grip;
};

#endif
