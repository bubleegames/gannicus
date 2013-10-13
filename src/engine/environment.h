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
	vector<unique_ptr<force>> globals;
	int grav;
};
#endif
