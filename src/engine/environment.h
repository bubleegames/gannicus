#ifndef __ENVIRONMENT
#define __ENVIRONMENT
#include "force.h"

class environment {
public:
	environment();
	void cleanup();
	void roundInit();
	void enforceGravity(instance *);
	void enforceGravity(player *);
	vector<force *> globals;
	int grav, floor, wall;
};
#endif
