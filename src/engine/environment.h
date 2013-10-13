#ifndef __ENVIRONMENT
#define __ENVIRONMENT
#include "force.h"

class environment {
public:
	environment();
	void cleanup();
	vector<force *> globals;
	int grav, floor, wall;
};
#endif
