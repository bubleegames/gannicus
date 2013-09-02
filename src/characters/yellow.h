#ifndef YELLOW_INCLUDED
#define YELLOW_INCLUDED
#include "../engine/character.h"
class yellow : public character {
public:
	yellow();
	void resetAirOptions(vector<int>&);
	action * createMove(string);
	int takeHit(status&, hStat&, int, int&);
	void drawMeters(int, int, status&);
	void tick(status&);
	void init(status&);
	vector<int> generateMeter();
	void step(status&);
};

class flashStep : public airMove {
public:
	flashStep();
	flashStep(string, string);
	bool check(const status&);
	void execute(status&);
private:
	int flashMeterCost;
};

class flashSummon : public special {
public:
	flashSummon();
	flashSummon(string, string);
	bool check(const status&);
	void execute(status&);
	bool setParameter(string);
	virtual void zero();
	void step(status&);
private:
	int flashMeterGain;
};
#endif
