#ifndef __ENVIRONMENT
#define __ENVIRONMENT
#include "force.h"
#include <memory>
#include <utility>
using std::shared_ptr;
using std::make_shared;

class environment {
public:
	environment();
	vector<shared_ptr<force>> physics;
	void cleanup();
	void roundInit();
	void airCheck(instance *);
	void enforce(instance *);
	void enforceFloor(instance *);
	void checkCorners(instance*);
	void enforceBounds(instance*);
	void load(string);
	void setParameter(string);
	int screenWidth, screenHeight;
	SDL_Rect bg;
	GLuint background;
	float bgR, bgG, bgB;
	int floor, wall;
};
#endif
