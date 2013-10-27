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
	void checkCorners(int, int, instance*);
	int floor, wall;
};
#endif
