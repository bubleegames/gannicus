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
	vector<shared_ptr<force>> globals;
	void cleanup();
	void roundInit();
	void enforceGravity(instance *);
	void enforceGravity(player *);
	int grav, floor, wall;
};
#endif
