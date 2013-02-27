#include "action.h"
utility::utility(const char * n)
{
	build(n);
}

bool utility::activate(std::vector<int> inputs, int pattern, int t, int f, int * meter, SDL_Rect &p)
{
	for(unsigned int i = 0; i < inputs.size(); i++){
		if(pattern & (1 << i)){
			if(inputs[i] < minHold) return 0;
			if(maxHold && inputs[i] > maxHold) return 0;
		} else if(inputs[i] == 1) return 0;
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return check(p, meter);
}

looping::looping(const char * n)
{
	build(n);
}

void looping::step(int *& meter, int &f, int &c, int &h)
{
	action::step(meter, f, c, h);
	if(f != 0){ 
		if(meter[1] + gain[0] < 300) meter[1] += gain[0];
		else meter[1] = 300;
	}
	if(f >= frames) f = 0;
}

