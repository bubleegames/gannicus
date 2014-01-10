#ifndef YELLOW_INCLUDED
#define YELLOW_INCLUDED
#include "../engine/character.h"
class yellow : public character {
public:
	yellow();
	action * createMove(string);
	int takeHit(status&, hStat&, int, int&);
	vector<HUDMeter<int>> drawMeters(int, status&);
	void tick(status&);
	void init(status&);
	vector<HUDMeter<int>> generateMeter();
	void step(status&);
};

class flashStep : public airMove {
public:
	flashStep();
	flashStep(string, string);
	bool check(const status&);
	action * execute(status&);
private:
	int flashMeterCost;
};

class flashSummon : public special {
public:
	flashSummon();
	flashSummon(string, string);
	bool check(const status&);
	action * execute(status&);
	bool setParameter(string);
	virtual void zero();
	void step(status&);
private:
	int flashMeterGain;
};
#endif
