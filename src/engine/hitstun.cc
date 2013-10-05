#include "action.h"
#include <math.h>

using std::max;

void hitstun::step(status &current)
{
	if(current.counter >= 0){
		current.frame = frames-1;
		action::step(current);
	} else {
		if(current.frame < frames - 1) action::step(current);
		current.counter++;
	}
}

bool hitstun::canGuard(int f)
{
	return (blockState.i & 7) ? true : false;
}

int hitstun::takeHit(hStat& s, int b, status& current)
{
	blockField temp;
	temp.i = s.blockMask.i;
	if(b == -2 && temp.i) temp.b.air = true;
	if(temp.i & blockState.i){
		switch (b){
		case -2:
			current.frame = 0;
			current.counter = -(s.stun - 2 - s.stun/4);
			s.push = 0;
			return -2;
		case -1:
			current.frame = 0;
			current.counter = -(s.stun - 1 - s.stun/5);
			s.push = (s.push*4)/5;
			return -1;
		case 0:
			current.frame = 0;
			current.counter = -(s.stun - max(1, s.stun/11));
			return 0;
		}
	}
	return 1;
}

hitstun::hitstun(string dir, string file)
{
	build(dir, file);
}
