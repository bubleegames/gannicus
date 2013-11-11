#include "action.h"

utility::utility(string dir, string file)
{
	build(dir, file);
}

bool utility::patternMatch(vector<int> inputs, int pattern, int t, int f)
{
	for(unsigned int i = 0; i < inputs.size(); i++){
		if(pattern & (1 << i)){
			if(inputs[i] < minHold) return 0;
			if(maxHold && inputs[i] > maxHold) return 0;
		} else if(inputs[i] == 1) return 0;
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return 1;
}

looping::looping(string dir, string file)
{
	build(dir, file);
}

void looping::step(status &current)
{
	action::step(current);
	if(current.frame && !current.meter[4].value){
		if(current.meter[1].value + gain[0] < 300) current.meter[1].value += gain[0];
		else current.meter[1].value = 300;
	}
	if(current.frame >= frames) current.frame = 0;
}

