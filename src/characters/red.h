#ifndef RED_INCLUDED
#define RED_INCLUDED
#include "../engine/character.h"
class red : public character{
public:
	red();
	action * createMove(string);
	void tick(status&);
	vector<HUDMeter<int>> generateMeter();
	void step(status&);
	void init(status&);
	vector<status> temporalBuffer;
	instance * backup;
};

class redCancel : virtual public special{
public:
	redCancel() {}
	~redCancel();
	redCancel(string, string);
	bool check(const status&);
	int arbitraryPoll(int, int);
	action * execute(status&);
};

class redSuper : virtual public special{
public:
	redSuper();
	~redSuper();
	redSuper(string d, string f) { build(d,f); }
	int arbitraryPoll(int, int);
};
#endif
