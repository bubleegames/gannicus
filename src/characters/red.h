#ifndef RED_INCLUDED
#define RED_INCLUDED
#include "../engine/character.h"
class red : public character{
public:
	red();
	action * createMove(string);
	void step(status&);
	vector<status> temporalBuffer;
	instance * backup;
};

class redCancel : virtual public special{
public:
	redCancel() {}
	~redCancel();
	redCancel(string, string);
	int arbitraryPoll(int, int);
};

class redSuper : virtual public special{
public:
	redSuper();
	~redSuper();
	redSuper(string d, string f) { build(d,f); }
	int arbitraryPoll(int, int);
};
#endif
