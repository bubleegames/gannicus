#include "interface.h"
#include <fstream>
#include <iostream>

special::special(string dir, string file)
{
	build(dir, file);
}

negNormal::negNormal(string dir, string file)
{
	build(dir, file);
}

void negNormal::zero()
{
	action::zero();
	minHold = 0;
	maxHold = 0;
}

bool special::check(const status &current)
{
	if(cost > current.meter[1]) return 0;
	else return action::check(current);
}

bool negNormal::patternMatch(vector<int> inputs, int pattern, int t, int f)
{
	for(unsigned int i = 0; i < inputs.size(); i++){
		if(pattern & (1 << i)){
			if(inputs[i] != -1) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return 1;
}

bool special::patternMatch(vector<int> inputs, int pattern, int t, int f)
{
	for(unsigned int i = 0; i < inputs.size(); i++){
		if(pattern & (1 << i)){
			if(abs(inputs[i]) != 1) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return 1;
}

bool mash::patternMatch(vector <int> inputs, int pattern, int t, int f)
{
	int go = 0;
	if(action::patternMatch(inputs, pattern, t, f)){
		for(unsigned int i = 0; i < inputs.size(); i++){
			if(inputs[i] >= minHold){
				if(inputs[i] <= maxHold || !maxHold) go++;
			}
		}
		if(go >= buttons) return 1;
	}
	return 0;
}

bool releaseCheck::patternMatch(vector<int> inputs, int pattern, int t, int f){
	for(unsigned int i = 0; i < inputs.size(); i++){
		if(inputs[i] > 0) return 0;
	}
	return 1;
}

void mash::zero()
{
	action::zero();
	buttons = 1;
}

bool mash::setParameter(string buffer)
{
	tokenizer t(buffer, "\t: \n-");
	if(t() == "Buttons"){
		buttons = stoi(t("\t: \n-"));
		return true;
	} else return action::setParameter(buffer);
}

int werf::arbitraryPoll(int n, int f)
{
	switch (n){
	case 28:
		if(f == 0) return 1;
		break;
	case 27:
		return startPosX;
	case 26:
		return startPosY;
	default:
		break;
	}
	return action::arbitraryPoll(n, f);
}

bool werf::check(const status &current)
{
	if(current.prox.y != 0) return 0;
	if(current.prox.x > 0) return 0;
	return action::check(current);
}

bool luftigeWerf::check(const status &current)
{
	if(current.prox.y == 0) return 0;
	if(current.prox.x > 0) return 0;
	return action::check(current);
}

bool werf::setParameter(string buffer)
{
	tokenizer t(buffer, "\t: \n-");
	if (t() == "Position"){
		startPosX = stoi(t());
		startPosY = stoi(t("\t: \n"));
		return true;
	} else return action::setParameter(buffer);
}

bool luftigeWerf::setParameter(string buffer)
{
	tokenizer t(buffer, "\t: \n-");
	if(t() == "Landing") return airMove::setParameter(buffer);
	else return werf::setParameter(buffer);
}
