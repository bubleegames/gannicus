#ifndef __ENVIRONMENT
#define __ENVIRONMENT
#include "force.h"

class environment {
public:
	environment();
	vector<force *> globals;
	int grav, floor, wall;
};
#endif
