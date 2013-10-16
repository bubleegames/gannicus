#ifndef ___force
#define ___force
#include "player.h"
#include <tuple>
using std::tuple;
class force : public instance {
public:
	force() : x(0), y(0), type(0), radius(0), eventHorizon(0), effectCode(3), origin(nullptr), check(nullptr), grip(0) {}
	virtual void enforce(instance&) = 0;
	tuple<SDL_Rect, SDL_Rect, float, bool> enforceBegin(const instance&);
	void enforceStop(instance&, SDL_Rect&);
	int x, y; //X-Yvalue. For globals this is only influenced by facingness, for local it is attractive force (negative for repulsive) based on mean Y
	int length; //How long the force holds
	int ID; //Who the force affects
	unsigned int type:2; //Type check variable. Type 0 is global, type 1 is linear decay, type 2 is half-life, and type 3 is a flat cut-off.
	int radius;/*The radius at which the vector decay. Irrelevant (unchecked) for type 0, is the distance at which the vector loses one
				*from its absolute value in type 1, the half-life point for type 2, and the cut-off point for type 3.
				*/
	int eventHorizon;
	int effectCode;
	instance * origin;
	action * check;
	int grip;
};

shared_ptr<force> makeForce(int t);

class globalForce : public force
{
	void enforce(instance&);
};

class linearDecay: public force
{
	void enforce(instance&);
};

class halfLifeDecay: public force
{
	void enforce(instance&);
};

class cutoffDecay : public force
{
	void enforce(instance&);
};

#endif
