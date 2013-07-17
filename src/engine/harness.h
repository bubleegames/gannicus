#include "analytics.h"

#ifndef ___harness
#define ___harness

using std::vector;
class command : public vector <inputs> {
public:
	void build(string);
	int condition;
};

class harness{
public:
	harness();
	virtual void init();
	virtual void readInput();
	virtual void processInput(SDL_Event&);
	virtual void runMacro();
	bool gameover;
	bool console;
	string macro;
	vector<controller*> p;
};

class arcadeHarness : public harness{
public:
	vector<frame> currentFrame;
	vector<int> counter; //Basically just a delay for menu interaction
	command pending;
	virtual void initContainers(int, int); //Spawn the containers needed for input
	virtual void initContainers();	//Reinitialize input containers to 0
	virtual void runMacro();
//	virtual void processInput(SDL_Event&);	/*Accepts input into input containers, for use by anything that wants it*/
};
#endif
