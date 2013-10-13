#include "environment.h"
environment::environment()
{
	force gravity;
	gravity.x = 0;
	gravity.y = -6;
	gravity.type = 0;
	gravity.ID = 0;
	gravity.radius = 0;
	gravity.effectCode = 3;
	gravity.posX = 0;
	gravity.posY = 0;
	gravity.length = -1;
	globals.push_back(&gravity);
}
