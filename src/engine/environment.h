#ifndef __ENVIRONMENT
#define __ENVIRONMENT
#include "force.h"
#include <memory>
#include <utility>
using std::unique_ptr;

template<typename T, typename ...Args>
unique_ptr<T> make_unique( Args&& ...args )
{
	    return std::unique_ptr<T>( new T( std::forward<Args&>(args)... ) );
}


class environment {
public:
	environment();
	void cleanup();
	void roundInit();
	void enforceGravity(instance *);
	void enforceGravity(player *);
	int grav, floor, wall;
	vector<unique_ptr<force>> globals;
};
#endif
