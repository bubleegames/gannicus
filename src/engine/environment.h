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
	int grav;
};
#endif
