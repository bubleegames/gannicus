#ifndef __ENVIRONMENT
#define __ENVIRONMENT
#include "auxil.h"
class environment {
public:
	environment();
	vector<force *> globals;
	int grav, floor, wall;
};
#endif
